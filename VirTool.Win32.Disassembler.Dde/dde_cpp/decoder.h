/*
       C_MODRM       for instructions that require MODRM
       C_PREFIX      for PREFIXES (SEG/REP)
       C_2BYTE       for instructions which opcode is 2 byte
*/
#define       C_SIZE1             0x01
#define       C_MODRM             0x02
#define       C_DATA8             0x04
#define       C_DATA16            0x08
#define       C_DATA32            0x10
#define       C_PREFIX            0x20
#define       C_2BYTE             0x40    
#define       C_66                0x66    //operand size prefix
#define       C_67                0x67    //address size prefix

#define       C_UNKNOWN           0x00

BYTE table_1[256]={
/* 00 */  C_MODRM
/* 01 */, C_MODRM
/* 02 */, C_MODRM
/* 03 */, C_MODRM
/* 04 */, C_DATA8    // add al, ax, eax + 32imm  w not set imm size = 8
/* 05 */, C_DATA32   // add al, ax, eax + 32imm  w set imm size = 32
/* 06 */, C_SIZE1    //push es
/* 07 */, C_SIZE1    //pop es
/* 08 */, C_MODRM    //or w set
/* 09 */, C_MODRM    //or w not set :)
/* 0A */, C_MODRM
/* 0B */, C_MODRM
/* 0C */, C_DATA8    // or al/ax/eax shortcut
/* 0D */, C_DATA32   // or al/ax/eax shortcut
/* 0E */, C_SIZE1    // push cs
/* 0F */, C_2BYTE
/* 10 */, C_MODRM    //adc w/d set mod rm, next 4
/* 11 */, C_MODRM
/* 12 */, C_MODRM
/* 13 */, C_MODRM
/* 14 */, C_DATA8    //adc al, imm8
/* 15 */, C_DATA32   //adc al, imm32
/* 16 */, C_SIZE1    //push ss
/* 17 */, C_SIZE1    //pop ss
/* 18 */, C_MODRM    //sbb w set/not set
/* 19 */, C_MODRM
/* 1A */, C_MODRM    //sbb d/w combinations
/* 1B */, C_MODRM    //sbb d/w combinations
/* 1C */, C_DATA8    //sbb al, imm8
/* 1D */, C_DATA32   //sbb eax, imm32
/* 1E */, C_SIZE1    //push ds
/* 1F */, C_SIZE1    //pop ds
/* 20 */, C_MODRM    //and mod/rm d/w bit combinations = 4
/* 21 */, C_MODRM
/* 22 */, C_MODRM
/* 23 */, C_MODRM    //end and mod/rm d/w bit combinations
/* 24 */, C_DATA8    //and al, imm8
/* 25 */, C_DATA32   //and al, imm32
/* 26 */, C_PREFIX
/* 27 */, C_SIZE1    //daa
/* 28 */, C_MODRM    //sub w/d mixup
/* 29 */, C_MODRM    
/* 2A */, C_MODRM
/* 2B */, C_MODRM    //sub w/d mixup end
/* 2C */, C_DATA8    //sub al, ax ,eax imm8
/* 2D */, C_DATA32   //sub al, ax, eax imm8
/* 2E */, C_PREFIX
/* 2F */, C_SIZE1    //das
/* 30 */, C_MODRM    //xor next 4
/* 31 */, C_MODRM
/* 32 */, C_MODRM
/* 33 */, C_MODRM
/* 34 */, C_DATA8    //xor al, ax, eax imm 8 w not set (al)
/* 35 */, C_DATA32   //xor al, ax, eax imm 32 w set (eax)
/* 36 */, C_PREFIX
/* 37 */, C_SIZE1           //AAA
/* 38 */, C_MODRM           //cmp d/w set 4 combinations
/* 39 */, C_MODRM           //cmp
/* 3A */, C_MODRM           //cmp
/* 3B */, C_MODRM           //cmp
/* 3C */, C_DATA8           //cmp al, ax, eax imm 8
/* 3D */, C_DATA32          //cmp al, ax, eax imm 32
/* 3E */, C_PREFIX
/* 3F */, C_SIZE1           //AAS
/* 40 */, C_SIZE1           //inc reg (alternate encoding)
/* 41 */, C_SIZE1
/* 42 */, C_SIZE1
/* 43 */, C_SIZE1
/* 44 */, C_SIZE1
/* 45 */, C_SIZE1
/* 46 */, C_SIZE1
/* 47 */, C_SIZE1
/* 48 */, C_SIZE1           //dec reg (alternate encoding)
/* 49 */, C_SIZE1
/* 4A */, C_SIZE1
/* 4B */, C_SIZE1
/* 4C */, C_SIZE1
/* 4D */, C_SIZE1
/* 4E */, C_SIZE1
/* 4F */, C_SIZE1
/* 50 */, C_SIZE1
/* 51 */, C_SIZE1
/* 52 */, C_SIZE1
/* 53 */, C_SIZE1
/* 54 */, C_SIZE1
/* 55 */, C_SIZE1
/* 56 */, C_SIZE1
/* 57 */, C_SIZE1
/* 58 */, C_SIZE1    //pop reg
/* 59 */, C_SIZE1
/* 5A */, C_SIZE1
/* 5B */, C_SIZE1
/* 5C */, C_SIZE1
/* 5D */, C_SIZE1
/* 5E */, C_SIZE1
/* 5F */, C_SIZE1    //pop reg ends
/* 60 */, C_SIZE1    //pushad
/* 61 */, C_SIZE1    //popad
/* 62 */, C_MODRM    //bound
/* 63 */, C_MODRM    //arpl
/* 64 */, C_PREFIX
/* 65 */, C_PREFIX
/* 66 */, C_PREFIX
/* 67 */, C_PREFIX
/* 68 */, C_DATA32  //push immidiate 32
/* 69 */, C_MODRM + C_DATA32       //imul reg/reg imm32
/* 6A */, C_DATA8   //push imm 8
/* 6B */, C_MODRM + C_DATA8        //imul reg/reg imm8
/* 6C */, C_SIZE1   //ins w not set
/* 6D */, C_SIZE1   //ins w set
/* 6E */, C_SIZE1   //outs w not set
/* 6F */, C_SIZE1   //outs w set
/* 70 */, C_DATA8    //jcc 8 bit displacement start
/* 71 */, C_DATA8
/* 72 */, C_DATA8
/* 73 */, C_DATA8
/* 74 */, C_DATA8
/* 75 */, C_DATA8
/* 76 */, C_DATA8
/* 77 */, C_DATA8
/* 78 */, C_DATA8
/* 79 */, C_DATA8
/* 7A */, C_DATA8
/* 7B */, C_DATA8
/* 7C */, C_DATA8
/* 7D */, C_DATA8
/* 7E */, C_DATA8
/* 7F */, C_DATA8    //jcc 8 bit displacement ends
/* 80 */, C_MODRM + C_DATA8   //sub immidira/reg 32bit imm, also cmp modrm/imm32, also cmp
/* 81 */, C_MODRM + C_DATA32   //sub imidiate/reg 32bit imm, also cmp modrm/imm32, also cmp
/* 82 */, C_MODRM + C_DATA8    //sub or mod rm 8imm w not set, --||--      /imm8, also cmp
/* 83 */, C_MODRM + C_DATA8    //sub or mod rm 8imm w set set, --||--      /imm8, also cmp
/* 84 */, C_MODRM    //test w not set
/* 85 */, C_MODRM    //test w set
/* 86 */, C_MODRM    //xchg w not set
/* 87 */, C_MODRM    //xchg w set
/* 88 */, C_MODRM    //mov not set w
/* 89 */, C_MODRM    //mov set w
/* 8A */, C_MODRM    //mov d set/not
/* 8B */, C_MODRM
/* 8C */, C_MODRM    //mov reg/seg
/* 8D */, C_MODRM    //lea
/* 8E */, C_MODRM    //mov reg/seg I guess
/* 8F */, C_MODRM    //pop reg/memory
/* 90 */, C_SIZE1    //nop
/* 91 */, C_SIZE1    //xchg al, ax, eax   reg
/* 92 */, C_SIZE1
/* 93 */, C_SIZE1
/* 94 */, C_SIZE1
/* 95 */, C_SIZE1
/* 96 */, C_SIZE1
/* 97 */, C_SIZE1    //xchg al, ax, eax,  ret ends
/* 98 */, C_SIZE1    //cbw, cwde
/* 99 */, C_SIZE1    //cdq, cwd
/* 9A */, C_DATA32 + C_DATA16 //far call (call unsinged full offset,selector)
/* 9B */, C_SIZE1    //wait//fwait
/* 9C */, C_SIZE1           //pushfd
/* 9D */, C_SIZE1           //popfd
/* 9E */, C_SIZE1           //sahf
/* 9F */, C_SIZE1           //LAHF
/* A0 */, C_DATA32          //mov al, ax, eax,mem full_offset
/* A1 */, C_DATA32          //mov al, ax, eax, mem full_offset
/* A2 */, C_DATA32          //mov mem, al, ax, eax full_offset
/* A3 */, C_DATA32          //mov mem, al, ax, eax full_offset
/* A4 */, C_SIZE1           //movsb
/* A5 */, C_SIZE1           //movsd
/* A6 */, C_SIZE1    //cmpsb... 2 of them w bit set
/* A7 */, C_SIZE1    
/* A8 */, C_DATA8    //test al/ax/eax shortcut
/* A9 */, C_DATA32   //test al/ax/eax shortcut
/* AA */, C_SIZE1    //stosb
/* AB */, C_SIZE1    //stosd, or stosw + PREFIX 66
/* AC */, C_SIZE1    //lodsb
/* AD */, C_SIZE1    //lodsw + PREFIX 66 or lodsd
/* AE */, C_SIZE1    //scasb
/* AF */, C_SIZE1    //scasd
/* B0 */, C_DATA8    //mov reg, imm 8 alterante encoding
/* B1 */, C_DATA8
/* B2 */, C_DATA8
/* B3 */, C_DATA8
/* B4 */, C_DATA8
/* B5 */, C_DATA8
/* B6 */, C_DATA8
/* B7 */, C_DATA8
/* B8 */, C_DATA32   //mov immidiate to reg (alternate encoding)
/* B9 */, C_DATA32
/* BA */, C_DATA32
/* BB */, C_DATA32
/* BC */, C_DATA32
/* BD */, C_DATA32
/* BE */, C_DATA32
/* BF */, C_DATA32
/* C0 */, C_MODRM+C_DATA8 //rcl reg/mem by imm8 also rcr depends on opcode in modr/m field, also rol/ror
/* C1 */, C_MODRM+C_DATA8 //-----------||-----------
/* C2 */, C_DATA16
/* C3 */, C_SIZE1    //ret no args
/* C4 */, C_MODRM    //les
/* C5 */, C_MODRM    //lds
/* C6 */, C_MODRM+C_DATA8   // mov mem/imm
/* C7 */, C_MODRM+C_DATA32  //litle change (mov mem/imm)
/* C8 */, C_DATA8 + C_DATA16    //enter 16disp, 8bit level =4 size
/* C9 */, C_SIZE1
/* CA */, C_DATA16              //retf param
/* CB */, C_SIZE1               //retf no param
/* CC */, C_SIZE1
/* CD */, C_DATA8    //int, 8bit interupt number
/* CE */, C_SIZE1    //into 
/* CF */, C_SIZE1    //iret
/* D0 */, C_MODRM    //rcl reg/memory by 1 , also rcr, also ror/rol
/* D1 */, C_MODRM    //same with w bit set , -||-
/* D2 */, C_MODRM    //rcl register by cl  , -||-
/* D3 */, C_MODRM    //same with w bit set , -||- , also rol,ror
/* D4 */, C_DATA8    //aam 2 byte long but C_DATA8 is processed as 2 in my algo so this is ok
/* D5 */, C_DATA8    //aad 2 bytes long C_DATA8 is processed as 2 byte long 
/* D6 */, C_SIZE1    //salc
/* D7 */, C_SIZE1    //xlat
/* D8 */, C_MODRM    //all FPU are C_MODRM
/* D9 */, C_MODRM    
/* DA */, C_MODRM
/* DB */, C_MODRM
/* DC */, C_MODRM
/* DD */, C_MODRM
/* DE */, C_MODRM
/* DF */, C_MODRM    //end FPU instructions
/* E0 */, C_DATA8    //loonz 8bit
/* E1 */, C_DATA8    //loopz 8bit
/* E2 */, C_DATA8    //loop  8bit
/* E3 */, C_DATA8    //jecxz 8bit Address-size prefix indicates jcxz or jecxz
/* E4 */, C_DATA8    //in al, port
/* E5 */, C_DATA8    //in eax, port
/* E6 */, C_DATA8    //out port w not set (out 0, al)
/* E7 */, C_DATA8    //out port w set (out 0, eax)
/* E8 */, C_DATA32
/* E9 */, C_DATA32    //jmp full displacement
/* EA */, C_DATA32 + C_DATA16 //jmp far full offset, selector
/* EB */, C_DATA8     //jmp 8 bit displacement
/* EC */, C_SIZE1    //in
/* ED */, C_SIZE1    //in
/* EE */, C_SIZE1    //out
/* EF */, C_SIZE1    //out
/* F0 */, C_PREFIX
/* F1 */, C_SIZE1    //int1
/* F2 */, C_PREFIX
/* F3 */, C_PREFIX
/* F4 */, C_SIZE1    //hlt
/* F5 */, C_SIZE1    //cmc
/* F6 */, C_MODRM + C_DATA8 //not (w not set), neg depends on opcode field in modrm, it can be test imm
/* F7 */, C_MODRM + C_DATA32 //not (w set)    , neg depends on opcode field in modrm, it can be test imm
/* F8 */, C_SIZE1    //clc
/* F9 */, C_SIZE1    //stc
/* FA */, C_SIZE1    //cli
/* FB */, C_SIZE1    //sti
/* FC */, C_SIZE1    //cld
/* FD */, C_SIZE1    //std    
/* FE */, C_MODRM    //inc/dec w not set (modrm)
/* FF */, C_MODRM    //inc/dec w set     (modrm) //call also depends on reg/opcode field
};                   //also jmp (depends on reg/opcode) fild

BYTE table_2[256]={
/* 00 */  C_MODRM           //lldt
/* 01 */, C_MODRM           //invlpg
/* 02 */, C_MODRM           //lar
/* 03 */, C_MODRM           //LSL
/* 04 */, 0
/* 05 */, 0
/* 06 */, C_SIZE1           //clts
/* 07 */, 0
/* 08 */, C_SIZE1           //invd
/* 09 */, C_SIZE1
/* 0A */, 0
/* 0B */, 0
/* 0C */, 0
/* 0D */, 0
/* 0E */, 0
/* 0F */, 0
/* 10 */, 0
/* 11 */, 0
/* 12 */, 0
/* 13 */, 0
/* 14 */, 0
/* 15 */, 0
/* 16 */, 0
/* 17 */, 0
/* 18 */, 0
/* 19 */, 0
/* 1A */, 0
/* 1B */, 0
/* 1C */, 0
/* 1D */, 0
/* 1E */, 0
/* 1F */, 0
/* 20 */, C_MODRM           //mov reg/crX
/* 21 */, C_MODRM           //mov drX/reg
/* 22 */, C_MODRM           //mov crX/reg and it foes all the way down
/* 23 */, C_MODRM           //mov reg/drX
/* 24 */, 0
/* 25 */, 0
/* 26 */, 0
/* 27 */, 0
/* 28 */, 0
/* 29 */, 0
/* 2A */, 0
/* 2B */, 0
/* 2C */, 0
/* 2D */, 0
/* 2E */, 0
/* 2F */, 0
/* 30 */, C_SIZE1
/* 31 */, C_SIZE1
/* 32 */, C_SIZE1
/* 33 */, C_SIZE1
/* 34 */, 0
/* 35 */, 0
/* 36 */, 0
/* 37 */, 0
/* 38 */, 0
/* 39 */, 0
/* 3A */, 0
/* 3B */, 0
/* 3C */, 0
/* 3D */, 0
/* 3E */, 0
/* 3F */, 0
/* 40 */, C_MODRM           //conditional move
/* 41 */, C_MODRM
/* 42 */, C_MODRM
/* 43 */, C_MODRM
/* 44 */, C_MODRM
/* 45 */, C_MODRM
/* 46 */, C_MODRM
/* 47 */, C_MODRM
/* 48 */, C_MODRM
/* 49 */, C_MODRM
/* 4A */, C_MODRM
/* 4B */, C_MODRM
/* 4C */, C_MODRM
/* 4D */, C_MODRM
/* 4E */, C_MODRM
/* 4F */, C_MODRM           //end conditional move 
/* 50 */, 0
/* 51 */, 0
/* 52 */, 0
/* 53 */, 0
/* 54 */, 0
/* 55 */, 0
/* 56 */, 0
/* 57 */, 0
/* 58 */, 0
/* 59 */, 0
/* 5A */, 0
/* 5B */, 0
/* 5C */, 0
/* 5D */, 0
/* 5E */, 0
/* 5F */, 0
/* 60 */, 0
/* 61 */, 0
/* 62 */, 0
/* 63 */, 0
/* 64 */, 0
/* 65 */, 0
/* 66 */, 0
/* 67 */, 0
/* 68 */, 0
/* 69 */, 0
/* 6A */, 0
/* 6B */, 0
/* 6C */, 0
/* 6D */, 0
/* 6E */, 0
/* 6F */, 0
/* 70 */, 0
/* 71 */, 0
/* 72 */, 0
/* 73 */, 0
/* 74 */, 0
/* 75 */, 0
/* 76 */, 0
/* 77 */, 0
/* 78 */, 0
/* 79 */, 0
/* 7A */, 0
/* 7B */, 0
/* 7C */, 0
/* 7D */, 0
/* 7E */, 0
/* 7F */, 0
/* 80 */, C_DATA32   //jccs 2 byte long imm32
/* 81 */, C_DATA32
/* 82 */, C_DATA32
/* 83 */, C_DATA32
/* 84 */, C_DATA32
/* 85 */, C_DATA32
/* 86 */, C_DATA32
/* 87 */, C_DATA32
/* 88 */, C_DATA32
/* 89 */, C_DATA32
/* 8A */, C_DATA32
/* 8B */, C_DATA32
/* 8C */, C_DATA32
/* 8D */, C_DATA32
/* 8E */, C_DATA32
/* 8F */, C_DATA32   //jccs 2byte long ends imm32
/* 90 */, C_MODRM
/* 91 */, C_MODRM
/* 92 */, C_MODRM
/* 93 */, C_MODRM
/* 94 */, C_MODRM
/* 95 */, C_MODRM
/* 96 */, C_MODRM
/* 97 */, C_MODRM
/* 98 */, C_MODRM
/* 99 */, C_MODRM
/* 9A */, C_MODRM
/* 9B */, C_MODRM
/* 9C */, C_MODRM
/* 9D */, C_MODRM
/* 9E */, C_MODRM
/* 9F */, C_MODRM
/* A0 */, C_SIZE1            //push fs
/* A1 */, C_SIZE1           //pop fs
/* A2 */, C_SIZE1           //cpuid
/* A3 */, C_MODRM           //bt reg/mem
/* A4 */, C_MODRM + C_DATA8
/* A5 */, C_MODRM
/* A6 */, 0
/* A7 */, 0
/* A8 */, C_SIZE1           //push gs
/* A9 */, C_SIZE1           //pop gs
/* AA */, C_SIZE1
/* AB */, C_MODRM           //bts
/* AC */, C_MODRM + C_DATA8
/* AD */, C_MODRM
/* AE */, 0
/* AF */, C_MODRM           //imul reg/reg or reg/mem
/* B0 */, C_MODRM           //cmpxchg
/* B1 */, C_MODRM           //cmpxchg
/* B2 */, C_MODRM           //lss
/* B3 */, C_MODRM           //btr
/* B4 */, C_MODRM           //lfs
/* B5 */, C_MODRM           //lgs
/* B6 */, C_MODRM           //movzx
/* B7 */, C_MODRM           //movzx
/* B8 */, 0
/* B9 */, 0
/* BA */, C_MODRM + C_DATA8 //bt imm8
/* BB */, C_MODRM           //btc mod/rm
/* BC */, C_MODRM           //BSF
/* BD */, C_MODRM           //BSR
/* BE */, C_MODRM           //movsx
/* BF */, C_MODRM           //movsx
/* C0 */, C_MODRM           //xadd
/* C1 */, C_MODRM           //xadd
/* C2 */, 0
/* C3 */, 0
/* C4 */, 0
/* C5 */, 0
/* C6 */, 0
/* C7 */, 0
/* C8 */, C_SIZE1           //bswap eax
/* C9 */, C_SIZE1
/* CA */, C_SIZE1
/* CB */, C_SIZE1
/* CC */, C_SIZE1
/* CD */, C_SIZE1
/* CE */, C_SIZE1
/* CF */, C_SIZE1           //bswap reg ends
/* D0 */, 0
/* D1 */, 0
/* D2 */, 0
/* D3 */, 0
/* D4 */, 0
/* D5 */, 0
/* D6 */, 0
/* D7 */, 0
/* D8 */, 0
/* D9 */, 0
/* DA */, 0
/* DB */, 0
/* DC */, 0
/* DD */, 0
/* DE */, 0
/* DF */, 0
/* E0 */, 0
/* E1 */, 0
/* E2 */, 0
/* E3 */, 0
/* E4 */, 0
/* E5 */, 0
/* E6 */, 0
/* E7 */, 0
/* E8 */, 0
/* E9 */, 0
/* EA */, 0
/* EB */, 0
/* EC */, 0
/* ED */, 0
/* EE */, 0
/* EF */, 0
/* F0 */, 0
/* F1 */, 0
/* F2 */, 0
/* F3 */, 0
/* F4 */, 0
/* F5 */, 0
/* F6 */, 0
/* F7 */, 0
/* F8 */, 0
/* F9 */, 0
/* FA */, 0
/* FB */, 0
/* FC */, 0
/* FD */, 0
/* FE */, 0
/* FF */, 0
};