/*
       DDE (deroko Disassembly/assembly engine)
       
       So far it works fine, bugs still unknown...
        
                                                 coded by deroko <deroko<at>gmail<dot>com>
                                                 http://deroko.headcoders.net

*/

#include<windows.h>
#include<stdio.h>

#include "decoder.h"
#include "dde.h"

DWORD dde_disassm(BYTE *, struct dde_struct *);
DWORD  dde_asm(BYTE *, struct dde_struct *);

int main(){ 
       DWORD i, j,  size;
       BYTE *memptr, *tempptr, *address; //*p;
       BYTE *assemble;
       dde_struct *dde, *temp;
       dde = (dde_struct *)VirtualAlloc(0, 4096, MEM_COMMIT, PAGE_READWRITE);
       assemble = (BYTE *)VirtualAlloc(0, 4096, MEM_COMMIT, PAGE_READWRITE);
       temp = dde;
       printf("\nDDE - deroko Disassembly/Assembly engine\n\n");
       printf("             deroko <deroko<at>gmail<dot>com>\n");
       printf("             http://deroko.headcoders.net\n");
       printf("\npress enter to start \n");
       getchar();
       
       address = (unsigned char *)GetProcAddress(LoadLibrary("kernel32"), "VirtualAllocEx");
       
       for (i = 0; i < 50; i++){ //changi this to decode more instructions
              size = dde_disassm(address, dde);
              //p=address;
              //for (j=0; j<size; j++)
              //      printf("%.2X ", p[j]);
              //printf("\n");
              dde++;        
              address = address + size;
          }
       printf("Disassembling donet\n");
       printf("Reassembling\n");
       
       dde = temp;
       
       while (dde->len != 0){
              size = dde_asm(assemble, dde);
              assemble += size;
              dde++;
       }
       printf ("Assembling done\n");
       getchar();
       
       ExitProcess(0);
       return 0;
}


DWORD dde_disassm(BYTE *startaddress, struct dde_struct *dde_p){
       BYTE opcode, *p, tempopcode,instruction;
       int size;
       WORD *imm16;
       DWORD *imm32;
       int opsizeprefix = 0, addressprefix = 0;
       size = 0;
       p=startaddress;
       opcode=*p;
       //Proces PREFIXES!!! if any...
       prefix_loop:    
              if (table_1[opcode] == C_PREFIX){
                     if (opcode == C_66){
                            opsizeprefix = 1;
                            dde_p->c66 = opcode;
                     }
                     if (opcode == C_67){
                            addressprefix = 1;
                            dde_p->c67 = opcode;
                     }
                     if ( (opcode == 0xF3) || (opcode == 0xF2))
                            dde_p->rep = opcode; 
                     if ( (opcode == 0x2e) || (opcode == 0x36) || (opcode == 0x3e) || (opcode == 0x26) ||
                            (opcode == 0x64) || (opcode == 0x65))
                            dde_p->cseg = opcode;  
                     if (opcode == 0xF0)
                            dde_p->lock = opcode; 
                     size++;
                     p++;
                     opcode=*p;
                     goto prefix_loop;
              }
              
       //check for test, stupid and fucking test
       instruction = table_1[opcode];
       if ((opcode == 0xF6) || (opcode == 0xF7)){
              opcode=p[1];
         if ((opcode & 0x38) != 0)         //test or neg/not...
                     instruction = C_MODRM;
         opcode=*p;  //restore old opcode
              
       }
       if (opsizeprefix)
              //where was 8 still is 8 //where was 32 now is 16
              if (instruction & C_DATA32){
                     instruction &= ~(C_DATA32);
                     instruction |= C_DATA16;
              }
       //not sure about this, but it works fine for now       
       if (addressprefix ){
                     instruction &= ~(C_DATA32);
                     instruction &= ~(C_DATA8);
                     instruction |= C_DATA16;
              }
       //fill instruction with flags from table_2 and decode it in right way
       if (instruction == C_2BYTE){
                  size += 1;
                  p++;
                  instruction = table_2[*p];
                  opcode = *p; 
                  dde_p->prefix_0F = 0x0F;
       }
       dde_p->opcode = opcode;       
       dde_p->flags = instruction;

       if (instruction & C_SIZE1)
                  size += 1;   
       if (instruction == (C_DATA8 + C_DATA16)){ //such as enter,16,0 for example
                  size +=  4;
                  p++;
                  imm32 = (DWORD *)p;
                  dde_p->imm.imm32 = *imm32;
              }
       if (instruction == (C_DATA16 + C_DATA32)) //far jmp, far call
                  size +=  7;                    //not handled by algo
       if (instruction == C_DATA32){
                  size +=  5;
                  p++;
                  imm32 = (DWORD *)p;
                  dde_p->imm.imm32 = *imm32;
              }
       if (instruction == C_DATA8){
                  size +=  2;
                  p++;
                  dde_p->imm.imm8 = *p;
              }
       if (instruction == C_DATA16){
                  size +=  3;
                  p++;
                  imm16 = (WORD *)p;
                  dde_p->imm.imm16 = *imm16;
              }
       if (instruction & C_MODRM){
                  unsigned char modrm;
                  size += 2;
                  p++;
                  modrm = *p;
                  dde_p->modrm = modrm;
                                                    
                  if ((modrm >> 6) != 3 && (modrm & 0x07) == 0x04){//{   //sib
                     size++;
                     p++;
                     dde_p->sib = *p;
                  }
                  //process modrm, check for 8/32 bit displacement...   
                  if ((modrm >> 6) == 0 && (modrm & 0x07) == 0x05){ //modrm is folowed by 32disp
                            size+=4;
                            p++;
                            imm32 = (DWORD *)p;
                            dde_p->disp.disp32 =(DWORD) *imm32;
                            dde_p->d32 = 1;
                     }
                  if (modrm >= 0x40 && modrm <= 0x7f){ //8bit displacement ?
                            size ++;
                            p++;
                            dde_p->disp.disp8 = *p;
                            dde_p->d8 = 1;
                     } 
                  if (modrm >= 0x80 && modrm <= 0xbf){ //32bit displacement ?
                            size +=4;                            
                            p++;
                            imm32 = (DWORD *)p;
                            dde_p->disp.disp32 = *imm32;
                            dde_p->d32 = 1;
                            p+=4;
                     }
                  p=(startaddress+size);
                  //p+=size;
                  //pprocess immidiate data if any!?   
                  if (instruction & C_DATA32){  //is this opcode opcode modrm immdata ???
                          size +=4 ;
                          imm32 = (DWORD *)p;
                          dde_p->imm.imm32 = *imm32;
                     }
                  if (instruction & C_DATA8){
                          size++;
                          dde_p->imm.imm8 = *p;
                     }
                  if (instruction & C_DATA16){
                          size+=2;        
                          imm16 = (WORD *)p;
                          dde_p->imm.imm16 = *imm16;
                     }
                     
       }
       
       if (instruction == C_UNKNOWN)
                  size = -1;
       dde_p->len = size;           
       return size;
}

//simple algo to reconstruct instructions :)
DWORD  dde_asm(BYTE *startaddress, struct dde_struct *dde_p){
       BYTE *p;
       DWORD *imm32;
       WORD *imm16;
       
       p=startaddress;
       if (dde_p->cseg){
              *p=dde_p->cseg;
               p++;
       }
       if (dde_p->c67){
              *p=dde_p->c67;
              p++;
       }
       if (dde_p->c66){
              *p=dde_p->c66;
              p++;
       }
       if (dde_p->rep){
              *p=dde_p->rep;
              p++;
       }
       if (dde_p->prefix_0F){
              *p=dde_p->prefix_0F;
              p++;
       }
////////////////// Make instructions, first check for modr/m sib and displacement if any       
       *p=dde_p->opcode;
       p++;
       if (dde_p->flags & C_MODRM){
              *p=dde_p->modrm;
              p++;
              if (dde_p->sib){
                     *p=dde_p->sib;
                     p++;
              }
              if (dde_p->d8){
                     *p = dde_p->disp.disp8;
                     p++;
              }
              if (dde_p->d32){
                     imm32 = (DWORD *)p;
                     *imm32 = dde_p->disp.disp32;
                     p+=4;
              }
              //process imm data after if any
              if (dde_p->flags & C_DATA8){
                     *p=dde_p->imm.imm8;
                     p++;
              }
              if (dde_p->flags & C_DATA16){
                     imm16 = (WORD *)p;
                     *imm16 = dde_p->imm.imm16;
                     p+=2;
              }
              if (dde_p->flags & C_DATA32){
                     imm32 = (DWORD *)p;
                     *imm32 = dde_p->imm.imm32;
                     p+=4;
              }
         }
/////process opcodes without C_MODRM such as C_DATA16, C_DATA8, C_DATA32
        if (dde_p->flags == C_DATA8){
              *p = dde_p->imm.imm8;
              p++;
       }
       if (dde_p->flags == C_DATA16){
              imm16 = (WORD *)p;
              *imm16 = dde_p->imm.imm16;
              //p+=2;
       }
       if (dde_p->flags == C_DATA32){
              imm32 = (DWORD *)p;
              *imm32 = dde_p->imm.imm32;
              //p+=4;
       }
       if (dde_p->flags == (C_DATA16 + C_DATA8)){ //enter 16,0 example processing
              imm32 = (DWORD *)p;
              *imm32 = dde_p->imm.imm32;
              //p+=3;
       }
       return dde_p->len;
}