#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ADE86.H"

extern "C"
{
int __cdecl
ASSEMBLE(Byte* memptr, Byte* maxptr, const char* source, Long org, int bits);

int __cdecl
UNASSEMBLE(void* binptr, void* maxptr, DIS86* result, Long org, int bits);

int __cdecl
PRINT_DIS86(void* outptr, void* maxptr,	DIS86* disasm, DBG86* debug);
}

int __cdecl
print_dis86(void* outptr, void* maxptr,	DIS86* disasm, DBG86* debug);

/* Disasssembler variables */
Long pos;			// Position in file
Long maxpos;			// Total file size
Byte usbuf[0x80];		// Buffer for command block
char imbuf[0x80];		// Buffer for instruction mnemonic

/* Assembler wariables */
Long org;			// Current origin
Long bits;			// Bits mode;
Long tsize;			// Total code size
Long nline;			// Current line number
Byte asmem[0x100];		// Memory for assembler
char lnmem[0x100];		// Source code buffer (current line)
bool asmode;			// Assemble mode


ASM86* as = (ASM86*)asmem;	// Result structure
DIS86* da = (DIS86*)asmem;

FILE* fin = stdin;		// Input file
FILE* fout = stdout;		// Output file


int
die(const char* msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}


bool
print_prompt()
{
	// Print command line prompt
	if(asmode==true && fin==stdin)
		fprintf(fout,"%08X: ",org);
	// Print file offset
	if(asmode==false)
		fprintf(fout,"%08X> ",pos);
	return true;
}


void
write_result()
{
	unsigned int n,p;
	// Show as hex if stream "stdout" or "stderr"
	if(fout==stdout || fout==stderr) {
		// Simple output for interactive mode
		if(fin==stdin) {
			fprintf(fout,"%10c",' ');
			for(n=0; n<as->bin_size; n++)
				fprintf(fout,"%02X",as->bin_output[n]);
			fputc('\n',fout);
		}
		// Listing output for file mode
		if(fin!=stdin && as->bin_size!=0) {
			p = fprintf(fout,"%08X  ",org-as->bin_size);
			for(n=0; n<as->bin_size; n++)
				p+=fprintf(fout,"%02X",as->bin_output[n]);
			for(n=p; n<40; n++)
				p+=fputc(' ',fout);
			fprintf(fout,"%s\n",lnmem);
		}
	} else {
		n = fwrite(as->bin_output,as->bin_size,1,fout);
		if(n != as->bin_size && ferror(fout))
			die("write_output()");
	}
}

int
process_directive()
{
	char *error=0,*pp=0;
	int p=0,n=0;

	// Pointer to first character
	while(isspace(lnmem[n]) && lnmem[n]!=0) n++;
	// Word length
	if(isalpha(lnmem[n])) {
		while(isalnum(lnmem[n+p])) p++;
	}

	if(p==5 && strnicmp(&lnmem[n],"USE16",p)==0) {
		bits = 16; return 1;
	}

	if(p==5 && strnicmp(&lnmem[n],"USE32",p)==0) {
		bits = 32; return 1;
	}

	if(p==3 && strnicmp(&lnmem[n],"ORG",p)==0) {
		if(strtoul(&lnmem[4],&pp,16)==0 && *pp!=0) {
			error = "Invalid origin value";
			goto return_error;
		}
		org = strtoul(&lnmem[4],&pp,16);
		return 1;
	}

	if(asmode==false && p==4 && strnicmp(&lnmem[n],"GOTO",p)==0) {
		if(strtoul(&lnmem[4],&pp,16)==0 && *pp!=0) {
			error = "Invalid offset value";
			goto return_error;
		}
		if(maxpos<strtoul(&lnmem[4],&pp,16)) {
			error = "Offset too lage";
			goto return_error;
		}
		pos = strtoul(&lnmem[4],&pp,16);
		return 1;
	}

	if(asmode==false && p==4 && strnicmp(&lnmem[n],"QUIT",p)==0) {
		fclose(fin);
		exit(EXIT_SUCCESS);
	}
	// No errors
	return 0;

return_error:

	// Continue in interactive mode
	if(fin==stdin || asmode==false) {
		fprintf(stdout,"\t  %s\n",error);
		return 1;
	}

	fprintf(stdout,">%s\n line %d: %s\n",lnmem,nline,error);
	return -1;
}


int
assemble_loop()
{
	Long n;
	while(print_prompt() && fgets(lnmem,sizeof(lnmem),fin) != 0) {
		nline++;
		// Replace '\n' to '\0'
		if(strchr(lnmem,'\n') != NULL)
			*strchr(lnmem,'\n') = '\0';
		// Process directive
		n=process_directive();
		// bad directive usage
		if(n==-1) break;
		// directive parsed
		if(n==1) continue;

		// Stop if void line in interactive mode
		if(fin == stdin && *lnmem=='\0')
			break;

		ASSEMBLE(asmem,asmem+sizeof(asmem),lnmem,org,bits);
		org += as->bin_size;
		tsize += as->bin_size;

		// error while assembling
		if(as->error != 0) {
			// Continue recv commands from "stdin"
			if(fin == stdin) {
				fprintf(stdout,"\t  %s\n",
					ade86_errors[as->error]);
				continue;
			}
			// Stop in file mode
			fprintf(stdout,">%s\n line %d: %s\n",
				lnmem,nline,ade86_errors[as->error]);
			break;
		}

		write_result();
	}

	// Exit with no messages in interactive mode
	if(fout == stdout || fout == stderr)
		return as->error;
	
	// Print compilation result in file mode
	fprintf(stdout,"Output: %d bytes\n",tsize);
	return as->error;
}


int
unassemble_to_source()
{
	Long n;
	fprintf(fout,"\t%s\n", bits==16? "USE16" : "USE32");
	while(pos<maxpos) {
		fseek(fin,pos,SEEK_SET)==0 || die("fseek");
		// Instruction buffer
		n = 30;
		if(pos+n > maxpos)
			n = maxpos-pos;
		// Fill buffer
		if(fread(usbuf,n,1,fin)!=(unsigned)n && ferror(fin))
			die("fread");
		UNASSEMBLE(usbuf,usbuf+n,da,org,bits);
		if(da->error!=0) {
			fprintf(fout,"error: %s\n",ade86_errors[da->error]);
			return EXIT_FAILURE;
		}
		memset(imbuf,0,sizeof(imbuf));
		PRINT_DIS86(imbuf,imbuf+sizeof(imbuf),da,0);
		fprintf(fout,"%s\n",imbuf);
		pos += da->size;
		org += da->size;		
	}
	return EXIT_SUCCESS;
}

int
unassemble_block(Long block)
{
	Long p,n,i,j,t;
	for(p=n=0; n<8 && p<block; n++)
	{
		UNASSEMBLE(&usbuf[p],usbuf+sizeof(usbuf),da,org,bits);
		memset(imbuf,0,sizeof(imbuf));
		if(da->error==E86_SUPERFLUOUS_PREFIX) {
			fprintf(fout,"%08X: %02X\n",org,usbuf[p]);
			p++; org++; pos++;
			continue;
		}
		if(da->error!=0) {
			fprintf(fout,"\t  %s\n",ade86_errors[da->error]);
			break;
		}
		for(t=i=0; i<da->size; i++)
			t+=sprintf(imbuf+t,"%02X",usbuf[p+i]);
		for(j=i; j<16; j++,t+=2)
			imbuf[t]=imbuf[t+1]=' ';
		PRINT_DIS86(imbuf+t,imbuf+sizeof(imbuf),da,0);
		fprintf(fout,"%08X: %s\n",org,imbuf);
		p += da->size;
		org += da->size;
		pos += da->size;
	}
	return 0;
}

int
unassemble_loop()
{
	Long n;
	while(print_prompt() && fgets(lnmem,sizeof(lnmem),stdin)!=0) {
		// Replace '\n' to '\0'
		if(strchr(lnmem,'\n') != NULL)
			*strchr(lnmem,'\n') = '\0';
		// Process directive
		n=process_directive();
		// directive parsed
		if(n==1 || n==-1) continue;
		// unassemble block if void line
		if(*lnmem!='\0')
			fprintf(fout,
				"\t  Unknown command\n"
				"\t  To unassemble simple press ENTER\n");

		fseek(fin,pos,SEEK_SET)==0 || die("fseek");
		// Get file block	
		n = sizeof(usbuf);
		if(pos+sizeof(usbuf) > maxpos)
			n = maxpos-pos;
		if(fread(usbuf,n,1,fin)!=(unsigned)n && ferror(fin))
			die("fread");

		unassemble_block(n);
	}
	return EXIT_SUCCESS;
}


int
main(int argc, char** argv)
{
	org = 0;
	pos = 0;
	bits = 32;
	nline = 0;
	int ap=1;

	if((--argc)) {
		bool unasm_mode = stricmp(argv[ap],"-U")==0 && ++ap;
		bool unasm_listing  = stricmp(argv[ap],"-T")==0 && ++ap;
		// Execute disassembler
		if(unasm_mode || unasm_listing) {
			// Check bits mode
			if(argc-1>0) {
				if(stricmp(argv[ap],"-16")==0) 
					bits = 16, ++ap, --argc;
				if(stricmp(argv[ap],"-32")==0)
					bits = 32, ++ap, --argc;
			}
			// Open target file
			(--argc) || die("No input file");
			fin = fopen(argv[ap],"rb");
			fin != NULL || die(argv[ap]);
			asmode = false;
			// Get file size
			fseek(fin,0,SEEK_END);
			maxpos = ftell(fin);
			fseek(fin,pos,SEEK_SET);
			// Print all instructions in text form
			if(unasm_listing)
				return unassemble_to_source();
			// Interactive mode
			return unassemble_loop();
		}
		// Input file
		fin = fopen(argv[1],"r");
		fin != NULL || die(argv[1]);
		// Output file
		if((--argc)) {
			fout = fopen(argv[2],"wb");
			fout != NULL || die(argv[2]);
		}
	}

	asmode = true;
	return assemble_loop();
}
