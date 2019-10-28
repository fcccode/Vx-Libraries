/*
Bill Prisoner Disassembler Engine v1.2 Source Code
*/
#include "NewTables.h"
#include <stdarg.h>
#define NULL 0
int _strlen(const char* Str1)
{
	int i=0;
	while (Str1[i]!=0)
	{
		i++;
	}
	return i;
}


int _strcpy(char* Str1,const char* Str2)
{
	int b = _strlen(Str2);
	for (int i=0;i<=b;i++)
	{
		Str1[i]=Str2[i];
	}
	return (int)Str1;
}

int _strcmp(const char* Str1,const char* Str2)
{
	if (_strlen(Str1)==_strlen(Str2))
	{
		for (int i=0;i<=_strlen(Str1);i++)
		{
			if (Str1[i]!=Str2[i]) return 1;
		}
		return 0;
	}
	return 1;
}

int _strcat(char *Str1,const char* Str2)
{
	Str1 = (char*)((int)Str1 + (int)_strlen(Str1));
	for (int i=0;i<=_strlen(Str2);i++)
	{
		Str1[i] = Str2[i];
	}
	return (int)Str1;
}

int _memset(void* Data,int Filled,int Size)
{
	for (int i=0;i<Size;i++)
	{
		((char*)Data)[i] = Filled;
	}
	return (int)Data;
}

char* ToHex(char* Buffer,int Number)
{
	int j=7;
	for (int i=0;i<4;i++)
	{
		char x = ((char*)&Number)[i];
		char x1 = x & 0x0f;//Младшая часть
		if (x1>=0x0 && x1<=0x9)
		{
			x1 = x1 + 0x30;
		}
		else
		{
			x1 = x1 + 0x37;
		}
		Buffer[j] = x1;
		j--;
		unsigned char x2 = ((char*)&Number)[i];
		x2 = x >> 4;//Старшая часть
		x2 = x2 & 0x0f;
		if (x2>=0x0 && x2<=0x9)
		{
			x2 = x2 + 0x30;
		}
		else
		{
			x2 = x2 + 0x37;
		}
		Buffer[j]=x2;
		j--;
	}
	Buffer[8]=0;
	int z = 0;
	for (i=0;i<_strlen(Buffer);i++)//Находим позицию для первого ненулевого
	{
		if (Buffer[i]!='0')
		{
			z = i;
			break;
		}
	}
	if (z==0)
	{
		if (Buffer[0]!='0')
		{
		}
		else
		{
			Buffer[0]='0';
			Buffer[1]='0';
			Buffer[2]=0;
		}
		return 0;
	}
	if (z==7)
	{
		Buffer[0]='0';
		Buffer[1]=Buffer[z];
		Buffer[2]=0;
		return 0;
	}
	char Temp;
	for (i=0;i<_strlen(Buffer)-z;i++)//Копируем
	{
		Temp = Buffer[z+i];
		Buffer[z+i] = Buffer[i];
		Buffer[i] = Temp;
		//Buffer[i]=Buffer[z+i];
	}
	Buffer[8-z]=0;
	return Buffer;
}

int _sprintf(char *Buffer,char* Format,...)
{
	va_list vl;
	va_start(vl,Format);
	int j=0;
	for (int i=0;i<=_strlen(Format);i++)
	{
		if (Format[i]!='%')
		{
			Buffer[j]=Format[i];
			j++;
		}
		else
		{
			i++;
			if (Format[i]=='s')
			{
				char* Str = va_arg(vl,char*);
				_strcpy(&Buffer[j],Str);
				j = j+_strlen(Str);
			}
			else
			if (Format[i]=='X')
			{
				int x = va_arg(vl,int);
				char Buffer2[3];
				ToHex(Buffer2,x);
				_strcpy(&Buffer[j],Buffer2);
				j = j + _strlen(Buffer2);
			}
		}
	}

	return 0;
}
//Main Opcode structure
typedef struct _Code
{
	short FieldPresents;//For checking presenting fields
	short Prefix;//Prefixes of instruction
	char OpCodeExt;//if first BYTE1 of opcode is 0FH
	short OpCodeWord;//if OpCode is WORD1 - in escape opcodes (or floating point opcodes)
	unsigned char OpCode;//OpCode BYTE
	unsigned char OpCode2;//If OpCode is Two Byte then this field is second byte in the opcode
	unsigned char ModRM;//ModRM BYTE 
	char SIB;//SIB BYTE
	unsigned long Immediate;//Immediate operand 
	unsigned long ImmediateEx;//Second Immediate operand (only ENTER instruction)
	unsigned long Displacement;//Displacement for operand
	unsigned short Segment;//Segment for Pointer operands
	unsigned long RelOffset;//Relative Offset in jump instructions
	long Offset;//Offset for Pointer Operands 
	char InstructionString[50];//Disassembled String
	char Group;//NumberOfGroup(1-16)
	long Flags;//Opcode Flags
	long Length;//Length of instruction
} Code;

//Getting SIB BYTE1
//Return Address next BYTE1 after SIB BYTE1
char* GetSIB(char* Offset,Code* Instruct)
{
	Instruct->FieldPresents = Instruct->FieldPresents | PSIB;
	Instruct->SIB = (char)*Offset;
	return ++Offset;
}

////////////////////////////////////////////////////
//Function Find Segment Register
long* FindSubStr(char* Str,long SS)
{
	for (int i=0;i<=_strlen((const char*)Str);i++)
	{
		long x;
		((char*)&x)[0] = Str[i];
		((char*)&x)[1] = Str[i+1];
		((char*)&x)[2] = Str[i+2];
		((char*)&x)[3] = Str[i+3];
		//Вот это блять извращенство. Это я блядь долго мучался что это гамно придумать.
		//Надо было сравнивать ворды по байтно. По другому ну никак.
		if (x == SS)
		{
			return (long*)&Str[i];
		}
	}
	return 0;
}

////////////////////////////////////////////////////
//Prefixes Handling
////////////////////////////////////////////////////
void PrefixHandling(Code* Instruction,char* Str)
{
	if ( (Instruction->Prefix & PREFIXMASK_CS)!=0)//Handling CS register
	{
		long* x = FindSubStr(Str,':SD ');
		if (x!=NULL) *x = ':SC ';
	}
	if ( (Instruction->Prefix & PREFIXMASK_ES)!=0)//Handling ES register
	{
		long* x = FindSubStr(Str,':SD ');
		if (x!=NULL) *x = ':SE ';
	}
	if ( (Instruction->Prefix & PREFIXMASK_FS)!=0)//Handling FS register
	{
		long* x = FindSubStr(Str,':SD ');
		if (x!=NULL) *x = ':SF ';
	}
	if ( (Instruction->Prefix & PREFIXMASK_SS)!=0)//Handling SS register
	{
		long* x = FindSubStr(Str,':SD ');
		if (x!=NULL) *x = ':SS ';
	}
	if ( (Instruction->Prefix & PREFIXMASK_GS)!=0)//Handling GS register
	{
		long* x = FindSubStr(Str,':SD ');
		if (x!=NULL) *x = ':SG ';
	}
	return;
}

////////////////////////////////////////////////////
//LOCK Handling
////////////////////////////////////////////////////
void LockHandling(Code* Instruction,char* Str)
{
	if ( (Instruction->Prefix & PREFIXMASK_LOCK)!=0)//Handling LOCK prefix
	{
		char Temp[50];
		_strcpy(Temp,Instruction->InstructionString);
		_sprintf(Instruction->InstructionString,"LOCK %s",Temp);
	}

}
////////////////////////////////////////////////////
//Repeat Handling
////////////////////////////////////////////////////
void RepeatHandling(Code* Instruction,char* Str)
{
	if ( (Instruction->OpCode>=0xa4 && Instruction->OpCode<=0xa7) || (Instruction->OpCode>=0xaa && Instruction->OpCode<=0xaf) )
	if ( (Instruction->Prefix & PREFIXMASK_REP)!=0)//Handling CS register
	{
		char Temp[50];
		_strcpy(Temp,Instruction->InstructionString);
		_sprintf(Instruction->InstructionString,"REP %s",Temp);
	}
	else
	if ( (Instruction->Prefix & PREFIXMASK_REPNZ)!=0)//Handling CS register
	{
		char Temp[50];
		_strcpy(Temp,Instruction->InstructionString);
		_sprintf(Instruction->InstructionString,"REPNZ %s",Temp);
	}
}

//Getting Displacement
char* GetDisplacement(char* Offset,Code* Instruct)
{
	Instruct->Displacement = 0;
	if ((Instruct->Flags & DISPLACEMENT8)!=0) //1 BYTE1 displacement for operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PDISP8;
		Instruct->Displacement = (char)*Offset;
		return ++Offset;
	}
	else
	if ((Instruct->Flags & DISPLACEMENT32)!=0 || (Instruct->Flags & DISPLACEMENT32_W)!=0 ) //4 BYTE1 displacement for operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PDISP32;
		unsigned long* x = (unsigned long*)Offset;
		Instruct->Displacement = *x;
		Offset+=4;
		return Offset;
	}
	else
	if ((Instruct->Flags & DISPLACEMENT16)!=0 ) //2 BYTE1 displacement for operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PDISP16;
		unsigned short* x = (unsigned short*)Offset;
		Instruct->Displacement = *x;
		Offset+=2;
		return Offset;
	}

	else
	{
		if ((Instruct->FieldPresents & PDISP32)!=0) 
		{
			unsigned long* x = (unsigned long*)Offset;
			Instruct->Displacement = *x;
			Offset+=4;
			return Offset;
		}
		if ((Instruct->FieldPresents & PDISP8)!=0)
		{
			Instruct->Displacement = (unsigned long)*Offset;
			return ++Offset;
		}
		if ((Instruct->FieldPresents & PDISP16)!=0) 
		{
			unsigned short* x = (unsigned short*)Offset;
			Instruct->Displacement = *x;
			Offset+=2;
			return Offset;
		}
	}
	return 0;
}

//Getting Pointer
char* GetPointer(char* Offset,Code* Instruct)
{
	Instruct->FieldPresents = Instruct->FieldPresents | PPOINT1;
	unsigned long* y = (unsigned long*)Offset;
	Instruct->Offset = *y;
	Offset+=4;
	unsigned short* x = (unsigned short*)Offset;
	Instruct->Segment = *x;
	Offset+=2;
	return Offset;
}

//Getting ImmediateOperand
//Return 1 if immediate is 1 BYTE1 (-)
//Return 0 if immediate is 4 bytes (-)
char* GetImmediate(char* Offset,Code* Instruct)
{
	Instruct->Immediate = 0;
	if (Instruct->Flags == (IMMEDIATE16 | IMMEDIATE8)) //Enter command - two immediate operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PIMM16;
		Instruct->FieldPresents = Instruct->FieldPresents | PIMM8;
		unsigned short* x = (unsigned short*)Offset;
		Instruct->Immediate = *x;
		Offset+=2;
		unsigned char* y = (unsigned char*)Offset;
		Offset+=1;
		Instruct->ImmediateEx = *y;
		return Offset;
	}
	if ((Instruct->Flags & IMMEDIATE8)!=0 || (Instruct->FieldPresents & PIMM8)!=0) //1 BYTE1 immediate operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PIMM8;
		Instruct->Immediate = (unsigned char)*Offset;
		Offset++;
	}
	if ((Instruct->Flags & IMMEDIATE32_W)!=0 || (Instruct->FieldPresents & PIMM32)!=0) //4 BYTE1 immediate operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PIMM32;
		unsigned long* x = (unsigned long*)Offset;
		Instruct->Immediate = *x;
		Offset+=4;
	}
	if ((Instruct->Flags & IMMEDIATE16)!=0) //2 BYTE1 immediate operand
	{
		Instruct->FieldPresents = Instruct->FieldPresents | PIMM16;
		unsigned short* x = (unsigned short*)Offset;
		Instruct->Immediate = *x;
		Offset+=2;
	}
	return Offset;
}

//Getting and analize ModRM BYTE1 
//Return next BYTE1 after modRm BYTE1 or after SIB BYTE1 if his presents 
char* GetModRM(char* Offset,Code* Instruct)
{
	int flag = 0;//Is only modrm - without disp and sib
	Instruct->ModRM = *Offset;//ModRM BYTE1
	Instruct->FieldPresents = Instruct->FieldPresents | PMODRM;
	//Now checking present or not SIB BYTE1 in instruction
	char Temp;//Temp for calculating Mod and Rm Fields
	char Temp2;//Temp for calculating SIB BYTE1 fields
	Temp = *Offset;//Temp constist of ModRM BYTE1

	if ((Temp & 0xC0)==0)//if (mod==00)
	{
		if ((Instruct->Prefix & PREFIXMASK_67)!=0 && ((Temp & 0x7)==0x6))//if (rm==110b)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PDISP16;
			return GetDisplacement(++Offset,Instruct);
		}
		else
		if ((Temp & 0x7)==0x5&&(Instruct->Prefix & PREFIXMASK_67)==0)//if (rm==101b)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PDISP32;
			return GetDisplacement(++Offset,Instruct);
		}
		if ((Temp & 0x7)==0x4 && (Instruct->Prefix & PREFIXMASK_67)==0)//if (rm==100b)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PSIB;
			Offset = GetSIB(++Offset,Instruct);
			flag=1;
			//SIB maybe include flag displacement32
			//if (base==101b)
			//&& (mod==00) then disp32
			Temp2 = Instruct->SIB;
			if ( (Temp2 & 0x7)==0x5 && (Temp & 0xC0)==0 )
			{
				Instruct->FieldPresents = Instruct->FieldPresents | PDISP32;
				Offset = GetDisplacement(Offset,Instruct);
				return Offset;
			}
		}
	}
	else
	if ((Temp & 0xC0)==0x40)//if (mod==01)
	{
		if ((Temp & 0x7)==0x4 && ((Instruct->Prefix & PREFIXMASK_67)==0))//if (rm==100b)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PSIB;
			Offset = GetSIB(++Offset,Instruct);
		}
		else 
			Offset++;
		flag = 1;
		Instruct->FieldPresents = Instruct->FieldPresents | PDISP8;
		return GetDisplacement(Offset,Instruct);
	}
	if ((Temp & 0xC0)==0x80)//if (mod==10)
	{
		if ((Temp & 0x7)==0x4 && ((Instruct->Prefix & PREFIXMASK_67)==0))//if (rm==100b)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PSIB;
			Offset = GetSIB(++Offset,Instruct);
		}
		else
			Offset++;
		if ((Instruct->Prefix & PREFIXMASK_67)!=0)
		{
			Instruct->FieldPresents = Instruct->FieldPresents | PDISP16;
		}
		else
			Instruct->FieldPresents = Instruct->FieldPresents | PDISP32;
		return GetDisplacement(Offset,Instruct);
	}
//	if ((Temp & 0xC0)==0xC0) //if (mod==11)
//	{
//		return ++Offset;
//	}
	if (flag == 0)
		++Offset;
	return Offset;
}

//Getting OpCode and next bytes
char* GetOpcode(char* Offset,Code* Instruct)
{
	Instruct->OpCode = *Offset;
	Instruct->Flags = OpcodesFlags[Instruct->OpCode];
	if ((Instruct->Flags & TWO)!=0)//Interpretation two byte opcodes
	{
		Instruct->OpCode = *Offset;
		Offset++;
		Instruct->OpCode2 = *Offset;
		Instruct->Flags = OpcodesFlagsTwo[Instruct->OpCode2];
		Instruct->FieldPresents = Instruct->FieldPresents | PTWO;
	}
	if ((Instruct->Prefix & PREFIXMASK_66)!=0)//If Operand Size Prefix is present
	{
		if ((Instruct->Flags & IMMEDIATE32_W)!=0)
		{
			Instruct->Flags = Instruct->Flags & ~IMMEDIATE32_W;//Deleting Immediate32 Flag
			Instruct->Flags = Instruct->Flags | IMMEDIATE16;
		}
		if ((Instruct->Flags & DWORD_W)!=0)
		{
			Instruct->Flags = Instruct->Flags & ~DWORD_W;//Deleting DWORD_W Flag
			Instruct->Flags = Instruct->Flags | WORD1;
		}
	}
	if ((Instruct->Prefix & PREFIXMASK_67)!=0)//If address Size Prefix is present
	{
		if ((Instruct->Flags & DISPLACEMENT32_W)!=0)
		{
			Instruct->Flags = Instruct->Flags & ~DISPLACEMENT32_W;//Deleting DISPLACEMENT32_W Flag
			Instruct->Flags = Instruct->Flags | DISPLACEMENT16;
		}
	}
	Offset++;//Next BYTE1 in instruction
	char TestIMM=0;
	if ((Instruct->Flags & GRP)!=0)//Reg Field ModRM BYTE - extension of Opcode
	{
		//Group1
		if (Instruct->OpCode>=0x80 && Instruct->OpCode<=0x83)
		{
			Instruct->Group = 1;
		}
		else
		//Group2
		if (Instruct->OpCode==0xC0 || Instruct->OpCode==0xC1 || Instruct->OpCode==0xD0 || Instruct->OpCode==0xD1 || Instruct->OpCode==0xD2 || Instruct->OpCode==0xD3)
		{
			Instruct->Group = 2;
		}
		else
		//Group11
		if (Instruct->OpCode==0xC6 || Instruct->OpCode==0xC7)
		{
			Instruct->Group = 11;
		}
		else
		if (Instruct->OpCode==0xf6 || Instruct->OpCode==0xf7)
		{
			Instruct->Group = 3;
		}
		else
		if (Instruct->OpCode==0xfe)
		{
			Instruct->Group = 4;
		}
		else
		if (Instruct->OpCode==0xff)
		{
			Instruct->Group = 5;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x00)
		{
			Instruct->Group = 6;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x01)
		{
			Instruct->Group = 7;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x18)
		{
			Instruct->Group = 16;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x72)
		{
			Instruct->Group = 13;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x71)
		{
			Instruct->Group = 12;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0x73)
		{
			Instruct->Group = 14;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0xae)
		{
			Instruct->Group = 15;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0xba)
		{
			Instruct->Group = 8;
		}
		else
		if ((Instruct->FieldPresents & PTWO)!=0 && Instruct->OpCode2==0xc7)
		{
			Instruct->Group = 9;
		}
	}
	if ((Instruct->Flags & OFFSET)!=0 && (Instruct->FieldPresents & PTWO)==0)//Relative Offset
	{
		if ((Instruct->Flags & BYTE1)!=0)//1 BYTE1
		{
			char x = (char)*Offset;
			Instruct->RelOffset = x+2;
			Offset++;
		}
		else
		if ((Instruct->Flags & DWORD_W)!=0)//4 BYTE1
		{
			unsigned long* x = (unsigned long*)Offset;
			Instruct->RelOffset = *x+5;
			Offset+=4;
		}
	}
	if ((Instruct->Flags & MODRM)!=0)//Getting ModRM BYTE1
	{
		Offset = GetModRM(Offset, Instruct);
		char Temp = Instruct->ModRM;
		if (Instruct->OpCode==0xF6)
		{
			if ((Temp & 56)==0) //reg == 000h
			{
				Instruct->FieldPresents = Instruct->FieldPresents | PIMM8;
				TestIMM = 1;
			}
		}
		else 
		if (Instruct->OpCode==0xF7)
		{
			if ((Temp & 56)==0) //reg == 000h
			{
				Instruct->FieldPresents = Instruct->FieldPresents | PIMM32;
				TestIMM = 1;
			}
		}
	}
	if ( ((Instruct->Flags & POINTER)!=0 || (Instruct->FieldPresents & PPOINT1)!=0) && (Instruct->FieldPresents & PTWO)==0)//Getting Pointer 
	{
		Offset = GetPointer(Offset,Instruct);
	}
	if ( ((Instruct->Flags & DISPLACEMENT8)!=0) || ((Instruct->Flags & DISPLACEMENT32_W)!=0)|| ((Instruct->Flags & DISPLACEMENT16)!=0))//Getting Displacement
	{
		Offset = GetDisplacement(Offset,Instruct);
	}
	if ( ((Instruct->Flags & IMMEDIATE8)!=0) || ((Instruct->Flags & IMMEDIATE32_W)!=0) || ((Instruct->Flags & IMMEDIATE16)!=0) || (TestIMM==1))//Getting Immediate operand
	{
		Offset = GetImmediate(Offset, Instruct);
	}
	return Offset;
}

//Return Address for next BYTE1 in instruction
//Function get Prefixes
//Return begin of OpCode
char* GetPrefixes(char* Offset,Code* Instruct)
{
	for (int i=0;i<=MAXSIZEOFOPCODE-1;i++)//Max command is 15 bytes
	{
		switch ((char)*Offset)
		{
			case PREFIX_CS:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_CS;
					break;
				}
			case PREFIX_SS:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_SS;
					break;
				}
			case PREFIX_DS:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_DS;
					break;
				}
			case PREFIX_ES:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_ES;
					break;
				}
			case PREFIX_FS:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_FS;
					break;
				}
			case PREFIX_GS:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKSEGREG) | PREFIXMASK_GS;
					break;
				}
			case PREFIX_LOCK:
				{
					Instruct->Prefix = Instruct->Prefix | PREFIXMASK_LOCK;
					break;
				}
			case PREFIX_REPNZ:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKREPEAT) | PREFIXMASK_REPNZ;
					break;
				}
			case PREFIX_REP:
				{
					Instruct->Prefix = (Instruct->Prefix & MASKREPEAT) | PREFIXMASK_REP;
					break;
				}
			case PREFIX_66:
				{
					Instruct->Prefix |= PREFIXMASK_66;
					break;
				}
			case PREFIX_67:
				{
					Instruct->Prefix |= PREFIXMASK_67;
					break;
				}
			default:
				return Offset;
		}
		Offset++;//Next BYTE1
	}
	return Offset;
}
//Print Prefixes in instruction
/*int PrintPrefixes(Code* Instr)
{
	short Prefix = Instr->Prefix;
	if ((Prefix & PREFIXMASK_LOCK)!=0) printf("PREFIX_LOCK IS FOUND.\n");
	if ((Prefix & PREFIXMASK_66)!=0) printf("PREFIX_66 IS FOUND.\n");
	if ((Prefix & PREFIXMASK_67)!=0) printf("PREFIX_67 IS FOUND.\n");
	if ((Prefix & PREFIXMASK_CS)!=0) printf("PREFIX_CS IS FOUND.\n");
	if ((Prefix & PREFIXMASK_DS)!=0) printf("PREFIX_DS IS FOUND.\n");
	if ((Prefix & PREFIXMASK_ES)!=0) printf("PREFIX_ES IS FOUND.\n");
	if ((Prefix & PREFIXMASK_SS)!=0) printf("PREFIX_SS IS FOUND.\n");
	if ((Prefix & PREFIXMASK_FS)!=0) printf("PREFIX_FS IS FOUND.\n");
	if ((Prefix & PREFIXMASK_GS)!=0) printf("PREFIX_GS IS FOUND.\n");
	if ((Prefix & PREFIXMASK_REP)!=0) printf("PREFIX_REP IS FOUND.\n");
	if ((Prefix & PREFIXMASK_REPNZ)!=0) printf("PREFIX_REPNZ IS FOUND.\n");
	if (Prefix==0) printf("PREFIXES IS NOT FOUND\n");
	//getch();
	return 0;
}*/

////////////////////////////////////////////////////
//Function Make Mnemonic
////////////////////////////////////////////////////
int MakeMnemonic(Code* Instruction)
{
	int SIB = 0;//For SIB BYTE1 interpretation and propusk ostalnoy part of code
	int None = 0;//For SIB BYTE1 scale index - none field
	int Nothing = 0;//For SIB BYTE1 Base = [*] interpretation
	int Unary = 0;//For Unary Group instruction
	int Group5Pointer = 0;//For 5 Group pointer
	int Direction = 0;//For changing direction
	int GenReg = 0;//If MMX or XMM instruction consist General purpose register
	int GenReg2 = 0;//If MMX or XMM instruction consist General purpose register, but other direction
	int MMX1 = 0;//For instruction where mmx and xmm registers
	char StringTemp[50] = "";
	char FirstOperand[50],SecondOperand[50];
	if ((Instruction->Flags & RESERVED)!=0)
	{
		_strcpy(Instruction->InstructionString,"RESERVED");
		return 0;
	}
	if ((Instruction->Flags & COP)!=0 && (Instruction->FieldPresents & PTWO)==0)//If this is not two byte and cop instruction set 
	{
		unsigned char Temp;
		if (Instruction->OpCode == 0xd8)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,D8[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,D8[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,D8[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,D8[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,D8[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,D8[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,D8[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,D8[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FADD",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCOM",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUB",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIV",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FMUL",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCOMP",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUBR",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIVR",FirstOperand,SecondOperand);
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xd9)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,D9[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,D9[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,D9[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,D9[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,D9[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,D9[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,D9[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FLD",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					if (Temp==0xd0)
						_sprintf(Instruction->InstructionString,"FNOP");
					else
						_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					if (Temp==0xe0)
						_sprintf(Instruction->InstructionString,"FCHS");
					else
					if (Temp==0xe1)
						_sprintf(Instruction->InstructionString,"FABS");
					else
					if (Temp==0xe4)
						_sprintf(Instruction->InstructionString,"FTST");
					else
					if (Temp==0xe5)
						_sprintf(Instruction->InstructionString,"FXAM");
					else
						_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					if (Temp==0xf0)
						_sprintf(Instruction->InstructionString,"F2XM1");
					else
					if (Temp==0xf1)
						_sprintf(Instruction->InstructionString,"FYL2X");
					else
					if (Temp==0xf2)
						_sprintf(Instruction->InstructionString,"FPTAN");
					else
					if (Temp==0xf3)
						_sprintf(Instruction->InstructionString,"FPATAN");
					else
					if (Temp==0xf4)
						_sprintf(Instruction->InstructionString,"FXTRACT");
					else
					if (Temp==0xf5)
						_sprintf(Instruction->InstructionString,"FPREM1");
					else
					if (Temp==0xf6)
						_sprintf(Instruction->InstructionString,"FDECSTP");
					else
					if (Temp==0xf7)
						_sprintf(Instruction->InstructionString,"FINCSTP");
					return 0;
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FXCHG",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					if (Temp==0xe8)
						_sprintf(Instruction->InstructionString,"FLD1");
					else
					if (Temp==0xe9)
						_sprintf(Instruction->InstructionString,"FLDL2T");
					else
					if (Temp==0xea)
						_sprintf(Instruction->InstructionString,"FLDL2E");
					else
					if (Temp==0xeb)
						_sprintf(Instruction->InstructionString,"FLDPI");
					else
					if (Temp==0xec)
						_sprintf(Instruction->InstructionString,"FLDDLG2");
					else
					if (Temp==0xed)
						_sprintf(Instruction->InstructionString,"FLDLN2");
					else
					if (Temp==0xee)
						_sprintf(Instruction->InstructionString,"FLDZ");
					else
					if (Temp==0xef)
						_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					if (Temp==0xf8)
						_sprintf(Instruction->InstructionString,"FPREM");
					else
					if (Temp==0xf9)
						_sprintf(Instruction->InstructionString,"FYL2XP1");
					else
					if (Temp==0xfa)
						_sprintf(Instruction->InstructionString,"FSQRT");
					else
					if (Temp==0xfb)
						_sprintf(Instruction->InstructionString,"FSINCOS");
					else
					if (Temp==0xfc)
						_sprintf(Instruction->InstructionString,"FRNDINT");
					else
					if (Temp==0xfd)
						_sprintf(Instruction->InstructionString,"FSCALE");
					else
					if (Temp==0xfe)
						_sprintf(Instruction->InstructionString,"FSIN");
					else
					if (Temp==0xff)
						_sprintf(Instruction->InstructionString,"FCOS");
					return 0;
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xda)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DA[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,DA[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DA[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DA[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,DA[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,DA[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,DA[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DA[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVB",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVBE",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVE",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVU",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					if (Temp==0xe9)
					{
						_sprintf(Instruction->InstructionString,"FUCOMPP");
					}
					else
					{
						_sprintf(Instruction->InstructionString,"RESERVED");	
					}
					return 0;
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xdb)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DB[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,DB[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DB[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DB[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,DB[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DB[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVBB",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVNBE",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					if (Temp==0xe2)
						_sprintf(Instruction->InstructionString,"FNCLEX");
					else
					if (Temp==0xe3)
						_sprintf(Instruction->InstructionString,"FNINIT");
					else
						_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCOMI",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVNE",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCMOVNU",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FUCOMI",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xdc)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DC[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,DC[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DC[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DC[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,DC[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,DC[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,DC[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DC[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FADD",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUBR",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIVR",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FMUL",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUB",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIV",SecondOperand,FirstOperand);
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xdd)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DD[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DD[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DD[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,DD[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,DD[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DD[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					if (Temp==0xc0)
						_strcpy(Instruction->InstructionString,"FFREE ST(0)");
					else
					if (Temp==0xc1)
						_strcpy(Instruction->InstructionString,"FFREE ST(1)");
					else
					if (Temp==0xc2)
						_strcpy(Instruction->InstructionString,"FFREE ST(2)");
					else
					if (Temp==0xc3)
						_strcpy(Instruction->InstructionString,"FFREE ST(3)");
					else
					if (Temp==0xc4)
						_strcpy(Instruction->InstructionString,"FFREE ST(4)");
					else
					if (Temp==0xc5)
						_strcpy(Instruction->InstructionString,"FFREE ST(5)");
					else
					if (Temp==0xc6)
						_strcpy(Instruction->InstructionString,"FFREE ST(6)");
					else
					if (Temp==0xc7)
						_strcpy(Instruction->InstructionString,"FFREE ST(7)");
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					if (Temp==0xd0)
						_strcpy(Instruction->InstructionString,"FST ST(0)");
					else
					if (Temp==0xd1)
						_strcpy(Instruction->InstructionString,"FST ST(1)");
					else
					if (Temp==0xd2)
						_strcpy(Instruction->InstructionString,"FST ST(2)");
					else
					if (Temp==0xd3)
						_strcpy(Instruction->InstructionString,"FST ST(3)");
					else
					if (Temp==0xd4)
						_strcpy(Instruction->InstructionString,"FST ST(4)");
					else
					if (Temp==0xd5)
						_strcpy(Instruction->InstructionString,"FST ST(5)");
					else
					if (Temp==0xd6)
						_strcpy(Instruction->InstructionString,"FST ST(6)");
					else
					if (Temp==0xd7)
						_strcpy(Instruction->InstructionString,"FST ST(7)");
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FUCOM",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					if (Temp==0xd8)
						_strcpy(Instruction->InstructionString,"FSTP ST(0)");
					else
					if (Temp==0xd9)
						_strcpy(Instruction->InstructionString,"FSTP ST(1)");
					else
					if (Temp==0xda)
						_strcpy(Instruction->InstructionString,"FSTP ST(2)");
					else
					if (Temp==0xdb)
						_strcpy(Instruction->InstructionString,"FSTP ST(3)");
					else
					if (Temp==0xdc)
						_strcpy(Instruction->InstructionString,"FSTP ST(4)");
					else
					if (Temp==0xdd)
						_strcpy(Instruction->InstructionString,"FSTP ST(5)");
					else
					if (Temp==0xde)
						_strcpy(Instruction->InstructionString,"FSTP ST(6)");
					else
					if (Temp==0xdf)
						_strcpy(Instruction->InstructionString,"FSTP ST(7)");
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					if (Temp==0xe8)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(0)");
					else
					if (Temp==0xe9)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(1)");
					else
					if (Temp==0xea)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(2)");
					else
					if (Temp==0xeb)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(3)");
					else
					if (Temp==0xec)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(4)");
					else
					if (Temp==0xed)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(5)");
					else
					if (Temp==0xee)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(6)");
					else
					if (Temp==0xef)
						_strcpy(Instruction->InstructionString,"FUCOMP ST(7)");
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xde)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DA[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,DA[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DA[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DA[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,DA[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,DA[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,DA[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DA[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FADDP",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUBRP",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIVRP",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FMULP",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					if (Temp==0xd9)
						_sprintf(Instruction->InstructionString,"FCOMPP");
					else
					{
						_sprintf(Instruction->InstructionString,"RESERVED");
						return 0;
					}
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FSUBP",SecondOperand,FirstOperand);
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FDIVP",SecondOperand,FirstOperand);
				}
				return 0;
			}
		}
		else
		if (Instruction->OpCode == 0xdf)
		{
			if (Instruction->ModRM>=0 && Instruction->ModRM<=0xBF)//in range
			{
				Temp = Instruction->ModRM;
				char Temp = Instruction->ModRM;
				Unary = 1;
				if ((Temp & 56)==0) //reg == 000h
				{
					_strcpy(Instruction->InstructionString,DF[0]);
				}
				else
				if ((Temp & 56)==8) //reg == 001h
				{
					_strcpy(Instruction->InstructionString,DF[1]);
				}
				else
				if ((Temp & 56)==16) //reg == 010h
				{
					_strcpy(Instruction->InstructionString,DF[2]);
				}
				else
				if ((Temp & 56)==24) //reg == 011h
				{
					_strcpy(Instruction->InstructionString,DF[3]);
				}
				else
				if ((Temp & 56)==32) //reg == 100h
				{
					_strcpy(Instruction->InstructionString,DF[4]);
				}
				else
				if ((Temp & 56)==40) //reg == 101h
				{
					_strcpy(Instruction->InstructionString,DF[5]);
				}
				else
				if ((Temp & 56)==48) //reg == 110h
				{
					_strcpy(Instruction->InstructionString,DF[6]);
				}
				else
				if ((Temp & 56)==56) //reg == 111h
				{
					_strcpy(Instruction->InstructionString,DF[7]);
				}
			}
			else
			{
				Temp = Instruction->ModRM;
				_strcpy(FirstOperand,"ST(0)");//FirstOperand
				if ((Temp & 0x0f)==0x00||(Temp & 0x0f)==0x08)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(0)");
				}
				else
				if ((Temp & 0x0f)==0x01||(Temp & 0x0f)==0x09)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(1)");
				}
				else
				if ((Temp & 0x0f)==0x02||(Temp & 0x0f)==0x0a)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(2)");
				}
				else
				if ((Temp & 0x0f)==0x03||(Temp & 0x0f)==0x0b)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(3)");
				}
				else
				if ((Temp & 0x0f)==0x04||(Temp & 0x0f)==0x0c)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(4)");
				}
				else
				if ((Temp & 0x0f)==0x05||(Temp & 0x0f)==0x0d)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(5)");
				}
				else
				if ((Temp & 0x0f)==0x06||(Temp & 0x0f)==0x0e)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(6)");
				}
				else
				if ((Temp & 0x0f)==0x07||(Temp & 0x0f)==0x0f)//SecondOperand
				{
					_strcpy(SecondOperand,"ST(7)");
				}
				//Instruction
				if (Temp>=0xc0 && Temp<=0xc7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xd0 && Temp<=0xd7)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe0 && Temp<=0xe7)
				{
					if (Temp==0xe0)
						_sprintf(Instruction->InstructionString,"FSTSW AX");
					else
					{
						_sprintf(Instruction->InstructionString,"RESERVED");
						return 0;
					}
				}
				else
				if (Temp>=0xf0 && Temp<=0xf7)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FCOMIP",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xc8 && Temp<=0xcf)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xd8 && Temp<=0xdf)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
					return 0;
				}
				else
				if (Temp>=0xe8 && Temp<=0xef)
				{
					_sprintf(Instruction->InstructionString,"%s %s,%s","FUCOMIP",FirstOperand,SecondOperand);
				}
				else
				if (Temp>=0xf8 && Temp<=0xff)
				{
					_sprintf(Instruction->InstructionString,"RESERVED");
				}
				return 0;
			}
		}
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2>=0x90 && Instruction->OpCode2<=0x9f) || (Instruction->OpCode2==0xae)||(Instruction->OpCode2==0xc7))
	{
		Unary=1;
	}
	if (((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0x50)) ||((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xc5))||((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xd7)))
	{
		GenReg = 1;
	}
	if (((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0x6e))||((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0x7e)))
	{
		GenReg2 = 1;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && ((Instruction->OpCode2 == 0x11)||(Instruction->OpCode2 == 0x13)\
		||(Instruction->OpCode2 == 0x17)||(Instruction->OpCode2 == 0x29) ||(Instruction->OpCode2 == 0x2b) || (Instruction->OpCode2 == 0x7e)\
		|| (Instruction->OpCode2 == 0x7f)||(Instruction->OpCode2 == 0xb0)||(Instruction->OpCode2 == 0xb1)||(Instruction->OpCode2 == 0xb3)||(Instruction->OpCode2 == 0xc4)\
		||(Instruction->OpCode2 == 0xe7)))
	{
		Direction = 1;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && ((Instruction->OpCode2>=0xa3 && Instruction->OpCode2<=0xa5) || (Instruction->OpCode2>=0xab && Instruction->OpCode2<=0xad)\
		||(Instruction->OpCode2==0xbb)|| (Instruction->OpCode2==0xC0)|| (Instruction->OpCode2==0xC1)||(Instruction->OpCode2==0xC3)))
	{
		Direction = 1;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0x2c || Instruction->OpCode2==0x2d))
	{
		MMX1 = 1;
	}
	else
	if (Instruction->OpCode==0xa5)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"MOVS DWORD PTR ES:[EDI],DWORD PTR DS:[ESI]");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"MOVS WORD PTR ES:[EDI],WORD PTR DS:[ESI]");
		LockHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	else
	if (Instruction->OpCode==0xa7)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"CMPS DWORD PTR DS:[ESI],DWORD PTR ES:[EDI]");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"CMPS WORD PTR DS:[ESI],WORD PTR ES:[EDI]");
		LockHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	else
	if (Instruction->OpCode==0xab)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"STOS DWORD PTR ES:[EDI],EAX");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"STOS WORD PTR ES:[EDI],AX");
		LockHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		return 0;	
	}
	else
	if (Instruction->OpCode==0xad)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"LODS EAX,DWORD PTR DS:[ESI]");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"LODS AX,WORD PTR DS:[ESI]");
		LockHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	else
	if (Instruction->OpCode==0xaf)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"SCAS EAX,DWORD PTR ES:[EDI]");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"SCAS AX,WORD PTR ES:[EDI]");
		LockHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		return 0;	
	}
	if (Instruction->OpCode==0x60)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"PUSHAD");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"PUSHAW");
		LockHandling(Instruction,Instruction->InstructionString);
		return 0; 
	}
	if (Instruction->OpCode==0x98)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"CWDE");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"CBW");
		LockHandling(Instruction,Instruction->InstructionString);
		return 0; 
	}
	if (Instruction->OpCode==0x99)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"CDQ");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"CWD");
		LockHandling(Instruction,Instruction->InstructionString);
		return 0; 
	}
	if (Instruction->OpCode==0x61)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
			_strcpy(Instruction->InstructionString,"POPAD");
		else
		if ((Instruction->Flags & WORD1)!=0)
			_strcpy(Instruction->InstructionString,"POPAW");
		LockHandling(Instruction,Instruction->InstructionString);
		return 0; 
	}
	if ((Instruction->Flags & EAX)!=0 && (Instruction->FieldPresents & PTWO)==0)
	{
			if ((Instruction->Flags & DWORD_W)!=0)
				_strcpy(FirstOperand,"EAX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"AX");
	}
	if ((Instruction->Flags & REG)!=0)
	{
		char Temp = Instruction->OpCode;
		if ((Instruction->FieldPresents & PTWO)!=0)
		{
			_strcpy(Instruction->InstructionString,OpcodesStringsTwo[Instruction->OpCode2]);//Copy Opcode string to mnemonic string
			Temp = Instruction->OpCode2;
		}
		else
			_strcpy(Instruction->InstructionString,OpcodesStrings[Instruction->OpCode]);//Copy Opcode string to mnemonic string
		if ((Temp & 0x7)==0x0)//if (base==000b) eax
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"EAX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"AX");
		}
		else
		if ((Temp & 0x7)==0x1)//if (base==001b) ecx
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"ECX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"CX");
		}
		else
		if ((Temp & 0x7)==0x2)//if (base==010b) edx
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"EDX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"DX");
		}
		else
		if ((Temp & 0x7)==0x3)//if (base==011b) ebx
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"EBX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"BX");
		}
		else
		if ((Temp & 0x7)==0x4)//if (base==100b) esp
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"ESP");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"SP");
		}
		else
		if ((Temp & 0x7)==0x5)//if (base==101b) esp
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"EBP");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"BP");
		}
		else
		if ((Temp & 0x7)==0x6)//if (base==110b) esi
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"ESI");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"SI");
		}
		else
		if ((Temp & 0x7)==0x7)//if (base==111b) edi
		{
			if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0)
				_strcpy(FirstOperand,"EDI");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(FirstOperand,"DI");
		}
		if (Instruction->OpCode>=0x91 && Instruction->OpCode<=0x97)
		{
			if ((Instruction->Flags & DWORD_W)!=0)
				_strcpy(SecondOperand,"EAX");
			else
			if ((Instruction->Flags & WORD1)!=0)
				_strcpy(SecondOperand,"AX");
			_sprintf(Instruction->InstructionString,"%s %s,%s",Instruction->InstructionString,SecondOperand,FirstOperand);
		}
		else
		if (Instruction->OpCode>=0xb8 && Instruction->OpCode<=0xbf)
		{
			_sprintf(Instruction->InstructionString,"%s %s,%X",Instruction->InstructionString,FirstOperand,Instruction->Immediate);
		}
		else
		_strcat(Instruction->InstructionString,FirstOperand);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	else
	if (Instruction->Group==1)//Group 1
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[1]);
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[3]);
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,Group1Strings[7]);
		}
	}
	else
	if (Instruction->Group==2)//Group 2
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[1]);
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[3]);
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,Group2Strings[7]);
		}
	}
	else
	if (Instruction->Group==11)//Group 11
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,"MOV");
		}
		else
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==3)//Group 3
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[3]);
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,Group3Strings[7]);
		}
	}
	else
	if (Instruction->Group==4)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group4Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group4Strings[1]);
		}
		else
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==5)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[1]);
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[3]);
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group5Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==6)
	{
		char Temp = Instruction->ModRM;
		Unary = 1;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[1]);
			Instruction->Flags = Instruction->Flags & ~WORD1;//Deleting Immediate32 Flag
			Instruction->Flags = Instruction->Flags | DWORD_W;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[3]);
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group6Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==7)
	{
		char Temp = Instruction->ModRM;
		Unary = 1;
		if ((Temp & 192)!=192) //if mod!=11b
		{
			if ((Temp & 56)==0) //reg == 000h
			{
				_strcpy(Instruction->InstructionString,Group7Strings[0]);
			}
			else
			if ((Temp & 56)==8) //reg == 001h
			{
				_strcpy(Instruction->InstructionString,Group7Strings[1]);
			}
			else
			if ((Temp & 56)==16) //reg == 010h
			{
				_strcpy(Instruction->InstructionString,Group7Strings[2]);
			}
			else
			if ((Temp & 56)==24) //reg == 011h
			{
				_strcpy(Instruction->InstructionString,Group7Strings[3]);
			}
			else
			if ((Temp & 56)==40) //reg == 101h
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
				return 0;				
			}
			else
			if ((Temp & 56)==56) //reg == 111h
			{
				_strcpy(Instruction->InstructionString,Group7Strings[7]);
				Instruction->Flags = Instruction->Flags & ~S;//Deleting S Flag
				Instruction->Flags = Instruction->Flags | BYTE1;
			}
		}
		else //if mod==11b
		{
			if ((Temp & 56)==8) //reg == 001h
			{
				if ((Instruction->Prefix & PREFIXMASK_66)!=0)
					_strcpy(Instruction->InstructionString,"MONITOR AX,CX,DX");
				else
					_strcpy(Instruction->InstructionString,"MONITOR EAX,ECX,EDX");
				LockHandling(Instruction,Instruction->InstructionString);
				return 0;
			}
			else
			if ( (Temp & 56)!=32 && (Temp & 56)!=48 )
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
				return 0;
			}
		}
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group7Strings[4]);
			Instruction->Flags = Instruction->Flags & ~S;//Deleting S Flag
			Instruction->Flags = Instruction->Flags | WORD1;
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group7Strings[6]);
			Instruction->Flags = Instruction->Flags & ~S;//Deleting S Flag
			Instruction->Flags = Instruction->Flags | WORD1;
		}
	}
	else
	if (Instruction->Group==16)
	{
		char Temp = Instruction->ModRM;
		Unary = 1;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,Group16Strings[0]);
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,Group16Strings[1]);
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group16Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,Group16Strings[3]);
		}
		else
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==12)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group12Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group12Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group12Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==13)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group13Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group13Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group13Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==14)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,Group14Strings[2]);
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group14Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if (Instruction->Group==15)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 192)!=192) //if mod!=11b
		{
			if ((Temp & 56)==0) //reg == 000h
			{
				_strcpy(Instruction->InstructionString,"FXSAVE");
			}
			else
			if ((Temp & 56)==8) //reg == 001h
			{
				_strcpy(Instruction->InstructionString,"FXRSTOR");
			}
			else
			if ((Temp & 56)==16) //reg == 010h
			{
				_strcpy(Instruction->InstructionString,"LDMXCSR");
			}
			else
			if ((Temp & 56)==24) //reg == 011h
			{
				_strcpy(Instruction->InstructionString,"STMXCSR");
			}
			else
			if ((Temp & 56)==32) //reg == 100h
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
			}
			else
			if ((Temp & 56)==40) //reg == 101h
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
			}
			else
			if ((Temp & 56)==48) //reg == 110h
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
			}
			else
			if ((Temp & 56)==56) //reg == 111h
			{
				_strcpy(Instruction->InstructionString,"CLFLUSH");
			}
		}
		else
		{
			if ((Temp & 56)==40) //reg == 101h
			{
				_strcpy(Instruction->InstructionString,"LFENCE");
				return 0;
			}
			else
			if ((Temp & 56)==48) //reg == 110h
			{
				_strcpy(Instruction->InstructionString,"MFENCE");
				return 0;
			}
			else
			if ((Temp & 56)==56) //reg == 111h
			{
				_strcpy(Instruction->InstructionString,"SFENCE");
				return 0;
			}
			else
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
			}
		}
	}
	else
	if (Instruction->Group==8)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 56)==0) //reg == 000h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==8) //reg == 001h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==16) //reg == 010h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==24) //reg == 011h
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Temp & 56)==32) //reg == 100h
		{
			_strcpy(Instruction->InstructionString,Group8Strings[4]);
		}
		else
		if ((Temp & 56)==40) //reg == 101h
		{
			_strcpy(Instruction->InstructionString,Group8Strings[5]);
		}
		else
		if ((Temp & 56)==48) //reg == 110h
		{
			_strcpy(Instruction->InstructionString,Group8Strings[6]);
		}
		else
		if ((Temp & 56)==56) //reg == 111h
		{
			_strcpy(Instruction->InstructionString,Group8Strings[7]);
		}
	}
	else
	if (Instruction->Group==9)
	{
		char Temp = Instruction->ModRM;
		if ((Temp & 192)!=192) //if mod!=11b
		{
			if ((Temp & 56)==8) //reg == 001h
			{
				_strcpy(Instruction->InstructionString,"CMPXCH8B");
			}
			else
			{
				_strcpy(Instruction->InstructionString,"RESERVED");
				return 0;
			}
		}
		else
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	else
	if ((Instruction->FieldPresents & PTWO)==0 && (Instruction->Flags & COP)==0)
		_strcpy(Instruction->InstructionString,OpcodesStrings[Instruction->OpCode]);//Copy Opcode string to mnemonic string
	else
	if ((Instruction->FieldPresents & PTWO)!=0)
		_strcpy(Instruction->InstructionString,OpcodesStringsTwo[Instruction->OpCode2]);//Copy Opcode string to mnemonic string
	//For ed,ef instructions
	if (Instruction->OpCode==0xed)
	{
		_sprintf(Instruction->InstructionString,"%s %s,DX",Instruction->InstructionString,FirstOperand);		
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	else
	if (Instruction->OpCode==0xef)
	{
		_sprintf(Instruction->InstructionString,"%s DX,%s",Instruction->InstructionString,FirstOperand);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	//For a0-a3 Instructions
	if (Instruction->OpCode==0xa0 || Instruction->OpCode==0xa1)
	{
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s BYTE PTR DS:[%X]",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s EAX,DWORD PTR DS:[%X]",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s AX,WORD PTR DS:[%X]",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		PrefixHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
	}
	if (Instruction->OpCode==0xa2 || Instruction->OpCode==0xa3)
	{
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s BYTE PTR DS:[%X],AL",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s DWORD PTR DS:[%X],EAX",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_sprintf(Instruction->InstructionString,"%s WORD PTR DS:[%X],AX",OpcodesStrings[Instruction->OpCode],Instruction->Displacement);
		}
		PrefixHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
	}
	//For Jcc $+Jb instructions and etc. instructions
	if ((Instruction->Flags & OFFSET)!=0 && (Instruction->FieldPresents & PTWO)==0)
	{
		if ((Instruction->RelOffset & 0x80000000)==0x80000000)//if disp - is signed then 
			_sprintf(Instruction->InstructionString,"%s $-%X",OpcodesStrings[Instruction->OpCode],-Instruction->RelOffset);
		else
			_sprintf(Instruction->InstructionString,"%s $+%X",OpcodesStrings[Instruction->OpCode],Instruction->RelOffset);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	//For Direct Address instructions 
	if ((Instruction->Flags & POINTER)!=0 && Instruction->OpCode!=0xc4 && (Instruction->FieldPresents & PTWO)==0)
	{
		_sprintf(Instruction->InstructionString,"%s %X:%X",OpcodesStrings[Instruction->OpCode],Instruction->Segment,Instruction->Offset);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	//If in string mnemonic present '%'
	for (int i=0;i<=_strlen(Instruction->InstructionString);i++)//Find in Opcode string - "%"
	{
		if (Instruction->InstructionString[i]=='%')
		{
			_strcpy(StringTemp,Instruction->InstructionString);
			if ((Instruction->Flags==IMMEDIATE8) \
				|| (Instruction->Flags==IMMEDIATE32_W) \
				|| (Instruction->Flags==IMMEDIATE16))//Instruction with one BYTE1 immediate
				_sprintf(Instruction->InstructionString,StringTemp,Instruction->Immediate);
			//For 6c-6f instructions
			if ( (Instruction->Flags & ESDI)!=0)
			{
				if ((Instruction->Flags & DWORD_W)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"DWORD PTR ES:[EDI]");
				}
				else 
				if ((Instruction->Flags & BYTE1)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"BYTE PTR ES:[EDI]");
				}
				else 
				if ((Instruction->Flags & WORD1)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"WORD PTR ES:[EDI]");
				}
			}
			if ((Instruction->Flags & DSSI)!=0)
			{
				if ((Instruction->Flags & DWORD_W)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"DWORD PTR DS:[ESI]");
				}
				else 
				if ((Instruction->Flags & BYTE1)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"BYTE PTR DS:[ESI]");
				}
				else 
				if ((Instruction->Flags & WORD1)!=0)
				{
					_sprintf(Instruction->InstructionString,StringTemp,"WORD PTR ES:[EDI]");
				}
				PrefixHandling(Instruction,Instruction->InstructionString);
				RepeatHandling(Instruction,Instruction->InstructionString);
			}
			LockHandling(Instruction,Instruction->InstructionString);
			return 0;
		}
	}
	//Opcode with one BYTE1 immediate
	if ((Instruction->Flags==IMMEDIATE8) \
		|| (Instruction->Flags==IMMEDIATE32_W) \
		|| (Instruction->Flags==IMMEDIATE16))//Instruction with one BYTE1 immediate
	{
		_sprintf(Instruction->InstructionString,"%s%X",Instruction->InstructionString,Instruction->Immediate);
		PrefixHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}

	if (Instruction->Flags==(IMMEDIATE8 | IMMEDIATE16))//For Enter Command
	{
		_sprintf(Instruction->InstructionString,"%s %X,%X",Instruction->InstructionString,Instruction->Immediate,Instruction->ImmediateEx);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
////////////////////////////////////////////////////
//ModRM BYTE interpretation
////////////////////////////////////////////////////
	if ((Instruction->FieldPresents & PMODRM)!=0)
	{
	char Temp = Instruction->ModRM;//Temp BYTE1 for ModRm interpretation
	if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & ONLYREG)!=0)
	{
		if ((Temp & 192)!=192) //if mod!=11b
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MEM)!=0)
	{
		if ((Temp & 192)==192) //if mod==11b
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
	}
	if ((Temp & 0x7)==0x0)//if (rm==000b) eax
	{
		if ((Instruction->Prefix & PREFIXMASK_67)!=0)
		{
			_strcpy(FirstOperand,"BX+SI");
		}
		else
			_strcpy(FirstOperand,"EAX");
	}
	else
	if ((Temp & 0x7)==0x1)//if (rm==001b) ecx
	{
		if ((Instruction->Prefix & PREFIXMASK_67)!=0)
		{
			_strcpy(FirstOperand,"BX+DI");
		}
		else
			_strcpy(FirstOperand,"ECX");
	}
	else
	if ((Temp & 0x7)==0x2)//if (rm==010b) edx
	{
		if ((Instruction->Prefix & PREFIXMASK_67)!=0)
		{
			_strcpy(FirstOperand,"BP+SI");
		}
		else
			_strcpy(FirstOperand,"EDX");
	}
	else
	if ((Temp & 0x7)==0x3)//if (rm==011b) ebx
	{
		if ((Instruction->Prefix & PREFIXMASK_67)!=0)
		{
			_strcpy(FirstOperand,"BP+DI");
		}
		_strcpy(FirstOperand,"EBX");
	}
	else
	if ((Instruction->Prefix & PREFIXMASK_67)!=0)
	{
		if ((Temp & 0x7)==0x4)//if rm==100b
		_strcpy(FirstOperand,"SI");
	}
	else 
	if ( (Temp & 0x7)==0x4 && (Temp & 192)==192)
	{
		_strcpy(FirstOperand,"ESP");
	}
	else
	if ( (Temp & 0x7)==0x4 && (Temp & 192)!=192 )//if (rm==100b)and(mod!=11) sib
	{
////////////////////////////////////////////////////
//SIB field interpretation
////////////////////////////////////////////////////
		Temp = Instruction->SIB;
////////////////////////////////////////////////////
//Base field interpretation
////////////////////////////////////////////////////
		if ((Temp & 0x7)==0x0)//if (base==000b) eax
		{
			_strcpy(FirstOperand,"EAX");
		}
		else
		if ((Temp & 0x7)==0x1)//if (base==001b) ecx
		{
			_strcpy(FirstOperand,"ECX");
		}
		else
		if ((Temp & 0x7)==0x2)//if (base==010b) edx
		{
			_strcpy(FirstOperand,"EDX");
		}
		else
		if ((Temp & 0x7)==0x3)//if (base==011b) ebx
		{
			_strcpy(FirstOperand,"EBX");
		}
		else
		if ((Temp & 0x7)==0x4)//if (base==100b) esp
		{
			_strcpy(FirstOperand,"ESP");
		}
		else
		if ((Temp & 0x7)==0x5)//if (base==101b) esp
		{
			//EXTENDED
			int Temp2 = Instruction->ModRM;
			if ((Temp2 & 192)==0) //if mod==00b
			{
				Nothing = 1;
				_strcpy(FirstOperand,"");
			}
			else
			{
				_strcpy(FirstOperand,"EBP");
			}
		}
		else
		if ((Temp & 0x7)==0x6)//if (base==110b) esi
		{
			_strcpy(FirstOperand,"ESI");
		}
		else
		if ((Temp & 0x7)==0x7)//if (base==111b) edi
		{
			_strcpy(FirstOperand,"EDI");
		}
////////////////////////////////////////////////////
//Index field interpretation
////////////////////////////////////////////////////
		if ((Temp & 56)==0)//if (index==000b) eax
		{
			_strcat(FirstOperand,"+EAX");
		}
		else
		if ((Temp & 56)==8)//if (index==001b) ecx
		{
			_strcat(FirstOperand,"+ECX");
		}
		else
		if ((Temp & 56)==16)//if (index==010b) edx
		{
			_strcat(FirstOperand,"+EDX");
		}
		else
		if ((Temp & 56)==24)//if (index==011b) ebx
		{
			_strcat(FirstOperand,"+EBX");
		}
		else
		if ((Temp & 56)==32)//if (index==100b) eax
		{
			////NONE
			None = 1;
		}
		else
		if ((Temp & 56)==40)//if (index==101b) ebp
		{
			_strcat(FirstOperand,"+EBP");
		}
		else
		if ((Temp & 56)==48)//if (index==110b) esi
		{
			_strcat(FirstOperand,"+ESI");
		}
		else
		if ((Temp & 56)==56)//if (index==111b) edi
		{
			_strcat(FirstOperand,"+EDI");
		}
		if (Nothing == 1)//Deleting "+" in "+reg"
		{
			for (int i=0;i<_strlen(FirstOperand);i++)
			{
				char Temp = FirstOperand[i];
				FirstOperand[i] = FirstOperand[i+1];
				FirstOperand[i+1] = Temp;
			}
		}
////////////////////////////////////////////////////
//Scalar field interpretation
////////////////////////////////////////////////////
		if (None == 0)
		{
			if ((Temp & 192)==0) //if ss==00b
			{
				//Nothing 
			}
			else
			if ((Temp & 192)==64) //if ss==01b
			{
				_strcat(FirstOperand,"*2");
			}
			else
			if ((Temp & 192)==128) //if ss==10b
			{
				_strcat(FirstOperand,"*4");
			}
			else
			if ((Temp & 192)==192) //if ss==11b
			{
				_strcat(FirstOperand,"*8");
			}
		}
		SIB = 1;
	}
	if (SIB==0)
	{
		if ((Temp & 0x7)==0x5)//if (rm==101b) ebp
		{
			if ((Instruction->Prefix & PREFIXMASK_67)!=0)
			{
				_strcpy(FirstOperand,"DI");
			}
			else
			{
				if ((Temp & 0xC0)==0)//if (mod==00)
					_sprintf(FirstOperand,"%X",Instruction->Displacement);
				else
					_strcpy(FirstOperand,"EBP");
			}
		}
		else
		if ((Temp & 0x7)==0x6)//if (rm==110b) esi
		{
			if ((Instruction->Prefix & PREFIXMASK_67)!=0)
			{
				if ((Temp & 0xC0)==0)//if (mod==00)
					_sprintf(FirstOperand,"%X",Instruction->Displacement);
				else
					_strcpy(FirstOperand,"BP");
			}
			else
				_strcpy(FirstOperand,"ESI");

		}
		else
		if ((Temp & 0x7)==0x7)//if (rm==111b) edi
		{
			if ((Instruction->Prefix & PREFIXMASK_67)!=0)
			{
				_strcpy(FirstOperand,"BX");
			}
			else
				_strcpy(FirstOperand,"EDI");
		}
	}
////////////////////////////////////////////////////
//Making Memory Address
////////////////////////////////////////////////////
	Temp = Instruction->ModRM;
	//Q: Temp & 0xC0 why?
	//R: For ModRm BYTE1 where one displacement because if (Temp&0xC0==0) FirstOperand==Displacement
	if ((Nothing==0)&&((Instruction->FieldPresents & PDISP8)!=0 || (Instruction->FieldPresents & PDISP32)!=0 || (Instruction->FieldPresents & PDISP16)!=0 && (Temp & 0xC0)!=0))
	{
		if ((Instruction->Displacement & 0x80000000)==0x80000000)//if disp - is signed then
		{
			_sprintf(StringTemp,"[%s-%X]",FirstOperand,-Instruction->Displacement);	
		}
		else
		{
			if ((Instruction->Displacement==0) || (Temp & 0xC0)==0)//if (mod==00)
				_sprintf(StringTemp,"[%s]",FirstOperand,Instruction->Displacement);
			else
			_sprintf(StringTemp,"[%s+%X]",FirstOperand,Instruction->Displacement);
		}
		_strcpy(FirstOperand,StringTemp);
	}
	else
	if ( (Instruction->FieldPresents & PDISP32)!=0 && Nothing==1) //For base == 101 && mod==00 resule [scaled index] + dis32
	{
		if ((Instruction->Displacement & 0x80000000)==0x80000000)//if disp - is signed then 
		{
			_sprintf(StringTemp,"[%s-%X]",FirstOperand,-Instruction->Displacement);		
		}
		else
		{
			if (Instruction->Displacement==0)
				_sprintf(StringTemp,"[%s]",FirstOperand,Instruction->Displacement);
			else
			_sprintf(StringTemp,"[%s+%X]",FirstOperand,Instruction->Displacement);
		}
		_strcpy(FirstOperand,StringTemp);
	}
	else
	if ((Temp & 192)==192) //if mod==11b
		_strcpy(StringTemp,FirstOperand);
	else
	{
		_sprintf(StringTemp,"[%s]",FirstOperand);
		_strcpy(FirstOperand,StringTemp);
	}
////////////////////////////////////////////////////
//Making Operand Size Prefix
////////////////////////////////////////////////////
	if ((Temp & 192)==192) //if mod==11b
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0 && (MMX1!=1) && (GenReg2!=1))
		{
			if ((Instruction->Prefix & PREFIXMASK_67)!=0)
			{
				if (_strcmp(StringTemp,"BX+SI")==0)	_strcpy(StringTemp,"MM0");
				else
				if (_strcmp(StringTemp,"BX+DI")==0)	_strcpy(StringTemp,"MM1");
				else
				if (_strcmp(StringTemp,"BP+SI")==0)	_strcpy(StringTemp,"MM2");
				else
				if (_strcmp(StringTemp,"BP+DI")==0)	_strcpy(StringTemp,"MM3");
				else
				if (_strcmp(StringTemp,"SI")==0)	_strcpy(StringTemp,"MM4");
				else
				if (_strcmp(StringTemp,"DI")==0)	_strcpy(StringTemp,"MM5");
				else
				if (_strcmp(StringTemp,"BP")==0)	_strcpy(StringTemp,"MM6");
				else
				if (_strcmp(StringTemp,"BX")==0)	_strcpy(StringTemp,"MM7");
			}
			else
			{
				if (_strcmp(StringTemp,"EAX")==0)	_strcpy(StringTemp,"MM0");
				else
				if (_strcmp(StringTemp,"ECX")==0)	_strcpy(StringTemp,"MM1");
				else
				if (_strcmp(StringTemp,"EDX")==0)	_strcpy(StringTemp,"MM2");
				else
				if (_strcmp(StringTemp,"EBX")==0)	_strcpy(StringTemp,"MM3");
				else
				if (_strcmp(StringTemp,"ESP")==0)	_strcpy(StringTemp,"MM4");
				else
				if (_strcmp(StringTemp,"EBP")==0)	_strcpy(StringTemp,"MM5");
				else
				if (_strcmp(StringTemp,"ESI")==0)	_strcpy(StringTemp,"MM6");
				else
				if (_strcmp(StringTemp,"EDI")==0)	_strcpy(StringTemp,"MM7");
			}
		}
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0)
		{
			if ((Instruction->Prefix & PREFIXMASK_67)!=0)
			{
				if (_strcmp(StringTemp,"BX+SI")==0)	_strcpy(StringTemp,"XMM0");
				else
				if (_strcmp(StringTemp,"BX+DI")==0)	_strcpy(StringTemp,"XMM1");
				else
				if (_strcmp(StringTemp,"BP+SI")==0)	_strcpy(StringTemp,"XMM2");
				else
				if (_strcmp(StringTemp,"BP+DI")==0)	_strcpy(StringTemp,"XMM3");
				else
				if (_strcmp(StringTemp,"SI")==0)	_strcpy(StringTemp,"XMM4");
				else
				if (_strcmp(StringTemp,"DI")==0)	_strcpy(StringTemp,"XMM5");
				else
				if (_strcmp(StringTemp,"BP")==0)	_strcpy(StringTemp,"XMM6");
				else
				if (_strcmp(StringTemp,"BX")==0)	_strcpy(StringTemp,"XMM7");
			}
			else
			{
				if (_strcmp(StringTemp,"EAX")==0)	_strcpy(StringTemp,"XMM0");
				else
				if (_strcmp(StringTemp,"ECX")==0)	_strcpy(StringTemp,"XMM1");
				else
				if (_strcmp(StringTemp,"EDX")==0)	_strcpy(StringTemp,"XMM2");
				else
				if (_strcmp(StringTemp,"EBX")==0)	_strcpy(StringTemp,"XMM3");
				else
				if (_strcmp(StringTemp,"ESP")==0)	_strcpy(StringTemp,"XMM4");
				else
				if (_strcmp(StringTemp,"EBP")==0)	_strcpy(StringTemp,"XMM5");
				else
				if (_strcmp(StringTemp,"ESI")==0)	_strcpy(StringTemp,"XMM6");
				else
				if (_strcmp(StringTemp,"EDI")==0)	_strcpy(StringTemp,"XMM7");
			}
		}
		else
		if ((Instruction->Prefix & PREFIXMASK_67)!=0)
		{
			if ((Instruction->Flags & BYTE1)!=0)
			{
				if (_strcmp(StringTemp,"BX+SI")==0)	_strcpy(StringTemp,"AL");
				else
				if (_strcmp(StringTemp,"BX+DI")==0)	_strcpy(StringTemp,"CL");
				else
				if (_strcmp(StringTemp,"BP+SI")==0)	_strcpy(StringTemp,"DL");
				else
				if (_strcmp(StringTemp,"BP+DI")==0)	_strcpy(StringTemp,"BL");
				else
				if (_strcmp(StringTemp,"SI")==0)	_strcpy(StringTemp,"AH");
				else
				if (_strcmp(StringTemp,"DI")==0)	_strcpy(StringTemp,"CH");
				else
				if (_strcmp(StringTemp,"BP")==0)	_strcpy(StringTemp,"DH");
				else
				if (_strcmp(StringTemp,"BX")==0)	_strcpy(StringTemp,"BH");
			}
			else
			if ((Instruction->Flags & WORD1)!=0)
			{
				if (_strcmp(StringTemp,"BX+SI")==0)	_strcpy(StringTemp,"AX");
				else
				if (_strcmp(StringTemp,"BX+DI")==0)	_strcpy(StringTemp,"CX");
				else
				if (_strcmp(StringTemp,"BP+SI")==0)	_strcpy(StringTemp,"DX");
				else
				if (_strcmp(StringTemp,"BP+DI")==0)	_strcpy(StringTemp,"BX");
				else
				if (_strcmp(StringTemp,"SI")==0)	_strcpy(StringTemp,"SP");
				else
				if (_strcmp(StringTemp,"DI")==0)	_strcpy(StringTemp,"BP");
				else
				if (_strcmp(StringTemp,"BP")==0)	_strcpy(StringTemp,"SI");
				else
				if (_strcmp(StringTemp,"BX")==0)	_strcpy(StringTemp,"DI");
			}
			else
			{
				if (_strcmp(StringTemp,"BX+SI")==0)	_strcpy(StringTemp,"EAX");
				else
				if (_strcmp(StringTemp,"BX+DI")==0)	_strcpy(StringTemp,"ECX");
				else
				if (_strcmp(StringTemp,"BP+SI")==0)	_strcpy(StringTemp,"EDX");
				else
				if (_strcmp(StringTemp,"BP+DI")==0)	_strcpy(StringTemp,"EBX");
				else
				if (_strcmp(StringTemp,"SI")==0)	_strcpy(StringTemp,"ESP");
				else
				if (_strcmp(StringTemp,"DI")==0)	_strcpy(StringTemp,"EBP");
				else
				if (_strcmp(StringTemp,"BP")==0)	_strcpy(StringTemp,"ESI");
				else
				if (_strcmp(StringTemp,"BX")==0)	_strcpy(StringTemp,"EDI");
			}
		}
		else
		{
			if ((Instruction->Flags & BYTE1)!=0||((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xb6||Instruction->OpCode2==0xbe)))
			{
				if (_strcmp(StringTemp,"EAX")==0)	_strcpy(StringTemp,"AL");
				else
				if (_strcmp(StringTemp,"ECX")==0)	_strcpy(StringTemp,"CL");
				else
				if (_strcmp(StringTemp,"EDX")==0)	_strcpy(StringTemp,"DL");
				else
				if (_strcmp(StringTemp,"EBX")==0)	_strcpy(StringTemp,"BL");
				else
				if (_strcmp(StringTemp,"ESP")==0)	_strcpy(StringTemp,"AH");
				else
				if (_strcmp(StringTemp,"EBP")==0)	_strcpy(StringTemp,"CH");
				else
				if (_strcmp(StringTemp,"ESI")==0)	_strcpy(StringTemp,"DH");
				else
				if (_strcmp(StringTemp,"EDI")==0)	_strcpy(StringTemp,"BH");
			}
			else
			if ((Instruction->Flags & WORD1)!=0 || ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0x02 || Instruction->OpCode2==0x03 || Instruction->OpCode2==0xb7 || Instruction->OpCode2==0xbf)))
			{
				if (_strcmp(StringTemp,"EAX")==0)	_strcpy(StringTemp,"AX");
				else
				if (_strcmp(StringTemp,"ECX")==0)	_strcpy(StringTemp,"CX");
				else
				if (_strcmp(StringTemp,"EDX")==0)	_strcpy(StringTemp,"DX");
				else
				if (_strcmp(StringTemp,"EBX")==0)	_strcpy(StringTemp,"BX");
				else
				if (_strcmp(StringTemp,"ESP")==0)	_strcpy(StringTemp,"SP");
				else
				if (_strcmp(StringTemp,"EBP")==0)	_strcpy(StringTemp,"BP");
				else
				if (_strcmp(StringTemp,"ESI")==0)	_strcpy(StringTemp,"SI");
				else
				if (_strcmp(StringTemp,"EDI")==0)	_strcpy(StringTemp,"DI");
			}
		}
		_strcpy(FirstOperand,StringTemp);
	}
	else
	if (Instruction->OpCode==0xFF)
	{
		Temp = Instruction->ModRM;
		if ((Temp & 56)==24 || (Temp & 56)==40) //reg == 011h
		{
			_sprintf(FirstOperand,"FWORD PTR DS:%s",StringTemp);	
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0)
		{
			_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_sprintf(FirstOperand,"WORD PTR DS:%s",StringTemp);
		}
		PrefixHandling(Instruction,FirstOperand);
//////////////////////////////////////////////////////////////////////////
	}
	else 
	if ((Instruction->Flags & A)!=0 && (Instruction->FieldPresents & PTWO)==0)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
		{
			_sprintf(FirstOperand,"QWORD PTR DS:%s",StringTemp);
		}
		else
		{
			_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
		}
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & POINTER)!=0&& (Instruction->FieldPresents & PTWO)==0)
	{
		if ((Instruction->Flags & DWORD_W)!=0)
		{
			_sprintf(FirstOperand,"FWORD PTR DS:%s",StringTemp);
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
		}
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & DWORD_W)!=0)
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xb6 || Instruction->OpCode2==0xbe))
		{
			_sprintf(FirstOperand,"BYTE PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xb7 || Instruction->OpCode2==0xbf))
		{
			_sprintf(FirstOperand,"WORD PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);
		}
		else
		if ( (Instruction->OpCode2==0x02 || Instruction->OpCode2==0x03) && (Instruction->FieldPresents & PTWO)!=0)
		{
			_sprintf(FirstOperand,"WORD PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);			
		}
		else
		if ((Temp & 192)!=192) 
		{
			_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);
		}
		else 
		{
			_sprintf(FirstOperand,"%s",StringTemp);
		}
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & DWORD1)!=0)
	{
		if ((Temp & 192)!=192) 
		{
			_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
		}
		else 
		{
			_sprintf(FirstOperand,"%s",StringTemp);
		}
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & BYTE1)!=0)
	{
		if ((Temp & 192)!=192) 
		{
			_sprintf(FirstOperand,"BYTE PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);
		}
		else 
			_sprintf(FirstOperand,"%s",StringTemp);
	}
	else
	if ((Instruction->Flags & WORD1)!=0)
	{
		if ((Temp & 192)!=192) 
		{
			_sprintf(FirstOperand,"WORD PTR DS:%s",StringTemp);
			PrefixHandling(Instruction,FirstOperand);
		}
		else 
			_sprintf(FirstOperand,"%s",StringTemp);
	}
	else
	if ((Instruction->Flags & S)!=0 && (Instruction->FieldPresents & PTWO)!=0)
	{
		_sprintf(FirstOperand,"FWORD PTR DS:%s",StringTemp);
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & PS)!=0 && (Instruction->FieldPresents & PTWO)!=0)
	{
		_sprintf(FirstOperand,"DQWORD PTR DS:%s",StringTemp);
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & Q)!=0 && (Instruction->FieldPresents & PTWO)!=0)
	{
		_sprintf(FirstOperand,"QWORD PTR DS:%s",StringTemp);
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & SS)!=0 && (Instruction->FieldPresents & PTWO)!=0)
	{
		_sprintf(FirstOperand,"DWORD PTR DS:%s",StringTemp);
		PrefixHandling(Instruction,FirstOperand);
	}
	else
	if ((Instruction->Flags & DQ)!=0 && (Instruction->FieldPresents & PTWO)!=0)
	{
		_sprintf(FirstOperand,"DQWORD PTR DS:%s",StringTemp);
		PrefixHandling(Instruction,FirstOperand);
	}

////////////////////////////////////////////////////
//Reg field implementation
////////////////////////////////////////////////////
	if (Instruction->Group==0)
	{
	if ((Temp & 56)==0)//if (reg==000b) eax
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(SecondOperand,"CR0");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR0");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1) && (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM0");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM0");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"ES");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"EAX");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"AL");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"AX");
		}
	}
	else
	if ((Temp & 56)==8)//if (reg==001b) ecx
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR1");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1) && (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM1");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM1");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"CS");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"ECX");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"CL");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"CX");
		}
	}
	else
	if ((Temp & 56)==16)//if (reg==010b) edx
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(SecondOperand,"CR2");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR2");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM2");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM2");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"SS");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"EDX");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"DL");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"DX");
		}
	}
	else
	if ((Temp & 56)==24)//if (reg==011b) ebx
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(SecondOperand,"CR3");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR3");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM3");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM3");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"DS");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"EBX");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"BL");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"BX");
		}
	}
	else
	if ((Temp & 56)==32)//if (reg==100b) esp
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(SecondOperand,"CR4");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM4");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM4");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"FS");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"ESP");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"AH");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"SP");
		}
	}
	else
	if ((Temp & 56)==40)//if (reg==101b) ebp
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM5");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM5");
		}
		else
		if ((Instruction->Flags & SEG)!=0 && (Instruction->FieldPresents & PTWO)==0)
		{
			_strcpy(SecondOperand,"GS");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"EBP");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"CH");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"BP");
		}
	}
	else
	if ((Temp & 56)==48)//if (reg==110b) esi
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR6");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM6");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM6");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"ESI");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"DH");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"SI");
		}
	}
	else
	if ((Temp & 56)==56)//if (reg==111b) edi
	{
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & CONTROLREG)!=0)
		{
			_strcpy(Instruction->InstructionString,"RESERVED");
			return 0;
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & DEBUGREG)!=0)
		{
			_strcpy(SecondOperand,"DR7");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & XMM)!=0 && (MMX1!=1)&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"XMM7");
		}
		else
		if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->Flags & MMX)!=0&& (GenReg!=1))
		{
			_strcpy(SecondOperand,"MM7");
		}
		else
		if ((Instruction->Flags & DWORD_W)!=0 || (Instruction->Flags & DWORD1)!=0 )
		{
			_strcpy(SecondOperand,"EDI");
		}
		else
		if ((Instruction->Flags & BYTE1)!=0)
		{
			_strcpy(SecondOperand,"BH");
		}
		else
		if ((Instruction->Flags & WORD1)!=0)
		{
			_strcpy(SecondOperand,"DI");
		}
	}
	}
	else
	if (Instruction->Group==2 && (Instruction->OpCode==0xD0 || Instruction->OpCode==0xD1))
	{
		_strcpy(SecondOperand,"1");
	}
	else
	if (Instruction->Group==2 && (Instruction->OpCode==0xD2 || Instruction->OpCode==0xD3))
	{
		_strcpy(SecondOperand,"CL");
	}
	else
	if (Instruction->OpCode==0xF6)
	{
		Temp = Instruction->ModRM;
		if ((Temp & 56)==0)//reg==000b
			_sprintf(SecondOperand,"%X",Instruction->Immediate);
		else 
		if ((Temp & 56)==8)//reg==001b
			;
		else
		if ((Temp & 56)==16)//reg==010b
		{
			Unary = 1;
		}
		else
		if ((Temp & 56)==24)//reg==011b
		{
			Unary = 1;
		}
		else
		if ((Temp & 56)==32)//reg==100b
		{
			_strcpy(SecondOperand,"AL");
		}
		else
		if ((Temp & 56)==40)//reg==101b
		{
			_strcpy(SecondOperand,"AL");
		}
		else
		if ((Temp & 56)==48)//reg==110b
		{
			_strcpy(SecondOperand,"AL");
		}
		else
		if ((Temp & 56)==56)//reg==110b
		{
			_strcpy(SecondOperand,"AL");
		}
	}
	else
	if (Instruction->OpCode==0xF7)
	{
		Temp = Instruction->ModRM;
		if ((Temp & 56)==0)//reg==000b
			_sprintf(SecondOperand,"%X",Instruction->Immediate);
		else 
		if ((Temp & 56)==8)//reg==001b
			;
		else
		if ((Temp & 56)==16)//reg==010b
		{
			Unary = 1;
		}
		else
		if ((Temp & 56)==24)//reg==011b
		{
			Unary = 1;
		}
		else
		if ((Temp & 56)==32)//reg==100b
		{
			_strcpy(SecondOperand,"EAX");
		}
		else
		if ((Temp & 56)==40)//reg==101b
		{
			_strcpy(SecondOperand,"EAX");
		}
		else
		if ((Temp & 56)==48)//reg==110b
		{
			_strcpy(SecondOperand,"EAX");
		}
		else
		if ((Temp & 56)==56)//reg==110b
		{
			_strcpy(SecondOperand,"EAX");
		}
	}
	else
	if (Instruction->Group==4)
	{
		Unary = 1;
	}
	else 
	if (Instruction->Group==5)
	{
		Unary = 1;
	}
	else
	if ((Instruction->Group==1)||(Instruction->Group==2) || (Instruction->Group==11))
	{
		_sprintf(SecondOperand,"%X",Instruction->Immediate);
	}
////////////////////////////////////////////////////
//Direction handling
////////////////////////////////////////////////////
	if ((Instruction->Flags & DIRECTION)!=0)
	{
		if ((Instruction->FieldPresents & PTWO)!=0)
			Temp = Instruction->OpCode2;
		else
			Temp = Instruction->OpCode;
		_strcpy(StringTemp,Instruction->InstructionString);//Copying basic mnemonic
		if ((Temp & 2)==0)//Direction == 1
		{
			_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,FirstOperand,SecondOperand);
		}
		else// Direction == 0
		{
			_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,SecondOperand,FirstOperand);
		}
		if ((Instruction->OpCode==0x69)||(Instruction->OpCode==0x6B)) _sprintf(Instruction->InstructionString,"%s,%X",Instruction->Immediate);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	_strcpy(StringTemp,Instruction->InstructionString);//Copying basic mnemonic
	if ( ((Instruction->Group==3) || (Instruction->Group==4)  || (Instruction->Group==5) || (Instruction->Group==7) \
		|| (Instruction->Group==6)||(Instruction->Group==15)||(Instruction->Group==16) || (Instruction->Group==9)||( (Instruction->FieldPresents & PTWO)!=0\
		&& (Instruction->OpCode2>=0x90 && Instruction->OpCode2<=0x9f)||(Instruction->OpCode==0xd8)||(Instruction->OpCode==0xd9)||\
		(Instruction->OpCode==0xda)||(Instruction->OpCode==0xdb)||(Instruction->OpCode==0xdC)||(Instruction->OpCode==0xdd\
		||(Instruction->OpCode==0xde)||(Instruction->OpCode==0xdf)))) && (Unary==1) )
	{
		_sprintf(Instruction->InstructionString,"%s %s",StringTemp,FirstOperand);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if (Instruction->Group==8)
	{
		_sprintf(Instruction->InstructionString,"%s %s,%X",StringTemp,FirstOperand,Instruction->Immediate);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if ((Instruction->Group==1)||(Instruction->Group==2)||(Instruction->Group==3))
	{
		_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,FirstOperand,SecondOperand);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if (Instruction->Group==11)
	{
		_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,FirstOperand,SecondOperand);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if (Instruction->Group==12 || Instruction->Group==13 || Instruction->Group==14)
	{
		_sprintf(Instruction->InstructionString,"%s %s,%X",StringTemp,FirstOperand,Instruction->Immediate);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if (Group5Pointer==1)
	{
		_sprintf(Instruction->InstructionString,"%s %X:%X",StringTemp,Instruction->Segment,Instruction->Offset);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	if (Instruction->OpCode==0x8f)
	{
		_sprintf(Instruction->InstructionString,"%s %s",StringTemp,FirstOperand);
		return 0;
	}
	if (Direction==0)
		_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,SecondOperand,FirstOperand);
	else 
		_sprintf(Instruction->InstructionString,"%s %s,%s",StringTemp,FirstOperand,SecondOperand);
	LockHandling(Instruction,Instruction->InstructionString);
	if ((Instruction->FieldPresents & PTWO)!=0 && Instruction->OpCode2==0x70)
	{
		_sprintf(Instruction->InstructionString,"%s,%X",Instruction->InstructionString,Instruction->Immediate);
		return 0;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2==0xc2 || Instruction->OpCode2==0xc4||Instruction->OpCode2==0xc5||Instruction->OpCode2==0xc6))
	{
		_sprintf(Instruction->InstructionString,"%s,%X",Instruction->InstructionString,Instruction->Immediate);
		return 0;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && Instruction->OpCode2==0xa4)
	{
		_sprintf(Instruction->InstructionString,"%s,%X",Instruction->InstructionString,Instruction->Immediate);
		return 0;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && Instruction->OpCode2==0xa5)
	{
		_sprintf(Instruction->InstructionString,"%s,%s",Instruction->InstructionString,"CL");
		return 0;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && Instruction->OpCode2==0xac)
	{
		_sprintf(Instruction->InstructionString,"%s,%X",Instruction->InstructionString,Instruction->Immediate);
		return 0;
	}
	if ((Instruction->FieldPresents & PTWO)!=0 && Instruction->OpCode2==0xad)
	{
		_sprintf(Instruction->InstructionString,"%s,%s",Instruction->InstructionString,"CL");
		return 0;
	}
	if ((Instruction->OpCode==0x69)||(Instruction->OpCode==0x6B))
	{
		_sprintf(Instruction->InstructionString,"%s,%X",Instruction->InstructionString,Instruction->Immediate);
		PrefixHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
		return 0;
	}
	}//ModRM
	if ((Instruction->OpCode>=0xa4 && Instruction->OpCode<=0xa7)||(Instruction->OpCode>=0xaa && Instruction->OpCode<=0xaf))
	{
		PrefixHandling(Instruction,Instruction->InstructionString);
		RepeatHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
	}
	else
	if ( (Instruction->Flags & IMMEDIATE8)!=0 || (Instruction->Flags & IMMEDIATE16)!=0 || (Instruction->Flags & IMMEDIATE32_W)!=0)
	{
		if (Instruction->OpCode==0xe7)
		{
			_sprintf(Instruction->InstructionString,"%s %X,%s",Instruction->InstructionString,Instruction->Immediate,FirstOperand);
		}
		else
			_sprintf(Instruction->InstructionString,"%s %s,%X",Instruction->InstructionString,FirstOperand,Instruction->Immediate);
		PrefixHandling(Instruction,Instruction->InstructionString);
		LockHandling(Instruction,Instruction->InstructionString);
	}
	else
	if ( (Instruction->FieldPresents & PTWO)!=0 && (Instruction->OpCode2>=0x80 && Instruction->OpCode2<=0x8f) )
		_sprintf(Instruction->InstructionString,"%s $+%X",Instruction->InstructionString,Instruction->Displacement+Instruction->Length);
	return 0;	
}

int disasm(char* Offset,Code* Instruction)
{
	_memset(Instruction,0,sizeof(Code));
	int x = (int)Offset;
	Offset = GetPrefixes(((char*)x),Instruction);
	int NumPrefix = Offset - (char*)x;
	if (NumPrefix>=15)
	{
		Instruction->Flags = RESERVED;
		Instruction->Length = 15;
	}
	else
		Instruction->Length = GetOpcode(Offset,Instruction) - (char*)x;
	if (Instruction->Length>15)
	{
		Instruction->Flags = RESERVED;
		Instruction->Length = 15;
	}
	MakeMnemonic(Instruction);
	return 0;
};