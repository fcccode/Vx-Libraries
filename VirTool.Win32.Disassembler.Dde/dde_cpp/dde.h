
struct        dde_struct {
       DWORD  len;          //instruction len
       BYTE   cseg;         //filled with seg reg prrefix otherwise=0
       BYTE   c66;          //filled with prefix 66h, otherwise = 0
       BYTE   c67;          //filled with prefix 67h, otherwise = 0
       BYTE   rep;          //F3 or F2 rep prefix, otherwise = 0
       BYTE   lock;         //lock prefix, otherwise = 0
       BYTE   opcode;       //opcode byte
       BYTE   prefix_0F;    //if 0F set to 0F, otherwise = 0 
       BYTE   flags;        //flags
       union{               //depends on flags set, dispositions for SIB (disp16 doesn't exist)
       DWORD  disp32;       
       BYTE   disp8;
       }disp;
       BYTE   d8;           //if disp8 is set d8 = 1, otherwise = 0
       BYTE   d32;          //if disp32 is set d32 = 1, otherwise = 0
       BYTE   modrm;        //modrm if any, otherwise = 0
       BYTE   sib;          //sib if any, othervise = 0
       union{               //immidiate data, depends on C_* flags in flags field
       DWORD  imm32;
       WORD   imm16;
       BYTE   imm8;
       }imm;
           
       BYTE   relative;            //not used yet
       BYTE   newopcode;           //original or inserted opcode
       BYTE   dummy1;              //not used yet
       BYTE   dummy2;              //not used yet
};

       