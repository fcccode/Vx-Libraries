/*
Constant and Strings File For BPDE
(C)oded by Bill Prisoner / TPOC
For Version 1.1
*/

#define MAXSIZEOFOPCODE 15
//Prefixes
const char PREFIX_CS    = 0x2E;
const char PREFIX_SS    = 0x36;
const char PREFIX_DS    = 0x3E;
const char PREFIX_ES    = 0x26;
const char PREFIX_FS    = 0x64;
const char PREFIX_GS    = 0x65;
const char PREFIX_LOCK  = 0xF0;
const char PREFIX_REPNZ = 0xF2;
const char PREFIX_REP   = 0xF3;
const char PREFIX_66    = 0x66; // redefenition operand size 
const char PREFIX_67    = 0x67; // redefenition address size
const char PREFIX_L_CD  = 0x0F; // two bytes on opcode

//Bits Mask for prefixes
const short PREFIXMASK_66    = 0x0001;
const short PREFIXMASK_67    = 0x0002;
const short PREFIXMASK_CS    = 0x0004;
const short PREFIXMASK_DS    = 0x0008;
const short PREFIXMASK_ES    = 0x0010;
const short PREFIXMASK_SS    = 0x0020;
const short PREFIXMASK_FS    = 0x0040;
const short PREFIXMASK_GS    = 0x0080;
const short PREFIXMASK_REP   = 0x0100;
const short PREFIXMASK_REPNZ = 0x0200;
const short PREFIXMASK_LOCK  = 0x0400;

//Prefixes Masks
const short MASKSEGREG = ~(PREFIXMASK_CS | PREFIXMASK_DS | PREFIXMASK_ES | PREFIXMASK_SS | PREFIXMASK_FS | PREFIXMASK_GS);//For zeroing all segment prefixes
const short MASKREPEAT = ~(PREFIXMASK_REP | PREFIXMASK_REPNZ);//For zeroing repeat prefixes

//Opcode flags
const long MODRM			=	0x00000001;//Mod R/M BYTE1 in instruction
const long DISPLACEMENT8_W	=	0x00000002;//One BYTE1 or WORD1 displacement depending on operand-size attribute
const long XMM				=	0x00000002;//Reg Field ModRm byte select one 128-bit XMM-register
const long DISPLACEMENT16_W	=	0x00000004;//Two or Four bytes displacement depending on operand-size attribute
const long DISPLACEMENT32_W	=	0x00000008;//Four bytes or Two bytes displacement depending on operand-size attribute
const long DISPLACEMENT8	=	0x00000010;//One BYTE1 displacement regardless on operand-size attribute
const long DISPLACEMENT16	=	0x00000020;//Two bytes displacement regardless on operand-size attribute
const long DISPLACEMENT32	=	0x00000040;//Four bytes displacement regardless on operand-size attribute
const long IMMEDIATE8		=	0x00000080;//One BYTE1 immediate regardless on operand-size attribute 
const long IMMEDIATE16		=	0x00000100;//Two bytes immediate regardless on operand-size attribute 
const long IMMEDIATE32		=	0x00000200;//Four bytes immediate regardless on operand-size attribute 
const long IMMEDIATE8_W		=	0x00000400;//One BYTE1 immediate depending on operand-size attribute
const long MMX				=	0x00000400;//MMX register
const long IMMEDIATE16_W	=	0x00000800;//Two bytes immediate depending on operand-size attribute
const long DQ				=	0x00000800;//DOUBLE QuadWOrd
const long IMMEDIATE32_W	=	0x00001000;//Four bytes immediate depending on operand-size attribute
const long DWORD_W			=	0x00002000;//Four bytes operand depending on operand-size attribute
const long WORD_W			=	0x00004000;//Two bytes operand depending on operand-size attribute 
const long BYTE1			=	0x00008000;//One BYTE1 operand regardless on operand-size attribute 
const long WORD1			=	0x00010000;//Two BYTE1 operand regardless on operand-size attribute 
const long DWORD1			=	0x00020000;//Four BYTE1 operand regardless on operand-size attribute 
const long MEM				=	0x00040000;//Operand maybe only memory, i.e. mod!=11
const long DSSI				=	0x00080000;//Address Of Operand is DS:ESI
const long Q				=	0x00080000;//Quadword regardless of operand-size attribute 
const long ESDI				=	0x00100000;//Address Of Operand is ES:EDI
const long SD				=	0x00100000;//Scalar element of a 128-bit packed double-precision floating data
const long POINTER			=	0x00200000;//Oparand is SEG:OFFSET32 or SEG:OFFSET16
const long SS				=	0x00200000;//Scalar element of a 128-bit packed single-precision floating data
const long DIRECTION		=	0x00400000;//Direction Flag
const long RESERVED 		=	0x00800000;//Is OpCode are reserved 
const long COP				=	0x01000000;//Escape to Coprocessor instruction set
const long PD				=	0x01000000;//128-bit packed double-precision floating-point data
const long TWO				=	0x02000000;//Escape to two bytes instruction set
const long DEBUGREG			=	0x02000000;//Debug Register in ModRm byte
const long A				=	0x04000000;//Two one-WORD1 operands in memory or two double-WORD1 operands in memory, depending on operand-size attribute
const long S				=	0x04000000;//For TwoByte Operands - operand is 6byte pseudo-descriptor
const long GRP				=	0x08000000;//reg Field ModRm BYTE1 is opcode extension
const long SEG				=	0x10000000;//reg Field ModRm BYTE1 select segment register
const long PS				=	0x10000000;//128-bit packed single-precision floating-point data
const long OFFSET			=	0x20000000;//Offset
const long CONTROLREG		=	0x20000000;//Control Register in ModRm byte
const long REG				=	0x40000000;//Reg Field into Opcode
const long EAX				=	0x80000000;//First operand - EAX or AL depending on operand size attribute
const long ONLYREG			=	0x80000000;//Operand in ModRm may described only registers,e.g. mod==11b

//Presents Flags 
const short PMODRM		=	0x0001;//
const short PIMM32		=	0x0002;//
const short PIMM8		=	0x0004;//
const short PPOINT1		=	0x0008;//
const short PDISP32		=	0x0010;//
const short PDISP8		=	0x0020;//
const short PSIB		=	0x0040;//
const short PIMM16		=	0x0080;//
const short PDISP16		=	0x0100;//
const short PTWO		=	0x0200;//

//Flags Of Two byte OpCodes ( Table A-3 )
const long OpcodesFlagsTwo[256] =
{
	MODRM | GRP | WORD1,//00h
	MODRM | GRP | S,	//01h
	MODRM | DWORD_W,	//02h
	MODRM | DWORD_W,	//03h
	RESERVED,			//04h
	RESERVED,			//05h
	0,					//06h
	RESERVED,			//07h
	0,					//08h
	0,					//09h
	RESERVED,			//0Ah
	0,					//0Bh
	RESERVED,			//0Ch
	RESERVED,			//0Dh
	RESERVED,			//0Eh
	RESERVED,			//0Fh

	MODRM | XMM | PS,	//10h
	MODRM | XMM | PS,	//11h
	MODRM | XMM | Q,	//12h
	MODRM | XMM | Q,	//13h
	MODRM | XMM | PS,	//14h
	MODRM | XMM | PS,	//15h
	MODRM | XMM | Q,	//16h
	MODRM | XMM | Q,	//17h
	GRP | MODRM | DWORD_W,//18h
	RESERVED,			//19h
	RESERVED,			//1Ah
	RESERVED,			//1Bh
	RESERVED,			//1Ch
	RESERVED,			//1Dh
	RESERVED,			//1Eh
	RESERVED,			//1Fh

	MODRM | ONLYREG | DIRECTION | CONTROLREG | DWORD1,	//20h
	MODRM | ONLYREG | DIRECTION | DEBUGREG | DWORD1,//21h
	MODRM | ONLYREG | DIRECTION | CONTROLREG | DWORD1,//22h
	MODRM | ONLYREG | DIRECTION | DEBUGREG | DWORD1,//23h
	RESERVED,				//24h
	RESERVED,				//25h
	RESERVED,				//26h
	RESERVED,				//27h
	MODRM | XMM | PS,		//28h
	MODRM | XMM | PS,		//29h
	MODRM | XMM | Q | MMX,	//2Ah
	MODRM | XMM | PS | MMX,	//2Bh
	MODRM | MMX | Q | XMM,	//2Ch
	MODRM | MMX | Q | XMM,	//2Dh
	MODRM | XMM | SS,		//2Eh
	MODRM | XMM | PS,		//2Fh

	0,						//30h
	0,						//31h
	0,						//32h
	0,						//33h
	0,						//34h
	0,						//35h
	RESERVED,				//36h
	RESERVED,				//37h
	RESERVED,				//38h
	RESERVED,				//39h
	RESERVED,				//3Ah
	RESERVED,				//3Bh
	RESERVED,				//3Ch
	RESERVED,				//3Dh
	RESERVED,				//3Eh
	RESERVED,				//3Fh

	MODRM | DWORD_W,		//40h
	MODRM | DWORD_W,		//41h
	MODRM | DWORD_W,		//42h
	MODRM | DWORD_W,		//43h
	MODRM | DWORD_W,		//44h
	MODRM | DWORD_W,		//45h
	MODRM | DWORD_W,		//46h
	MODRM | DWORD_W,		//47h
	MODRM | DWORD_W,		//48h
	MODRM | DWORD_W,		//49h
	MODRM | DWORD_W,		//4Ah
	MODRM | DWORD_W,		//4Bh
	MODRM | DWORD_W,		//4Ch
	MODRM | DWORD_W,		//4Dh
	MODRM | DWORD_W,		//4Eh
	MODRM | DWORD_W,		//4Fh

	MODRM | XMM | PS | DWORD1 | ONLYREG,//50h
	MODRM | XMM | PS,		//51h
	MODRM | XMM | PS,		//52h
	MODRM | XMM | PS,		//53h
	MODRM | XMM | PS,		//54h
	MODRM | XMM | PS,		//55h
	MODRM | XMM | PS,		//56h
	MODRM | XMM | PS,		//57h
	MODRM | XMM | PS,		//58h
	MODRM | XMM | PS,		//59h
	MODRM | XMM | Q,		//5Ah
	MODRM | XMM | DQ,		//5Bh
	MODRM | XMM | PS,		//5Ch
	MODRM | XMM | PS,		//5Dh
	MODRM | XMM | PS,		//5Eh
	MODRM | XMM | PS,		//5Fh

	MODRM | MMX | Q | DWORD1, //60h
	MODRM | MMX | Q | DWORD1,//61h
	MODRM | MMX | Q | DWORD1,//62h
	MODRM | MMX | Q | DWORD1,//63h
	MODRM | MMX | Q | DWORD1,//64h
	MODRM | MMX | Q | DWORD1,//65h
	MODRM | MMX | Q | DWORD1,//66h
	MODRM | MMX | Q | DWORD1,//67h
	MODRM | MMX | Q | DWORD1,//68h
	MODRM | MMX | Q | DWORD1,//69h
	MODRM | MMX | Q | DWORD1,//6Ah
	MODRM | MMX | Q | DWORD1,//6Bh
	RESERVED,				//6Ch
	RESERVED,				//6Dh
	MODRM | MMX | DWORD1,	//6Eh
	MODRM | MMX | Q | DWORD1,//6Fh

	MODRM | MMX | Q | DWORD1 | IMMEDIATE8,//70h
	MODRM | GRP | IMMEDIATE8 | MMX | Q,//71h
	MODRM | GRP | IMMEDIATE8 | MMX | Q,//72h
	MODRM | GRP | IMMEDIATE8 | MMX | Q,//73h
	MODRM | MMX | Q | DWORD1,//74h
	MODRM | MMX | Q | DWORD1,//75h
	MODRM | MMX | Q | DWORD1,//76h
	0,						//77h
	RESERVED,				//78h
	RESERVED,				//79h
	RESERVED,				//7Ah
	RESERVED,				//7Bh
	RESERVED,				//7Ch
	RESERVED,				//7Dh
	MODRM | MMX | DWORD1,	//7Eh
	MODRM | MMX | Q | DWORD1,//7Fh

	DISPLACEMENT32_W,		//80h
	DISPLACEMENT32_W,		//81h
	DISPLACEMENT32_W,		//82h
	DISPLACEMENT32_W,		//83h
	DISPLACEMENT32_W,		//84h
	DISPLACEMENT32_W,		//85h
	DISPLACEMENT32_W,		//86h
	DISPLACEMENT32_W,		//87h
	DISPLACEMENT32_W,		//88h
	DISPLACEMENT32_W,		//89h
	DISPLACEMENT32_W,		//8Ah
	DISPLACEMENT32_W,		//8Bh
	DISPLACEMENT32_W,		//8Ch
	DISPLACEMENT32_W,		//8Dh
	DISPLACEMENT32_W,		//8Eh
	DISPLACEMENT32_W,		//8Fh

	MODRM | BYTE1,			//90h
	MODRM | BYTE1,			//91h
	MODRM | BYTE1,			//92h
	MODRM | BYTE1,			//93h
	MODRM | BYTE1,			//94h
	MODRM | BYTE1,			//95h
	MODRM | BYTE1,			//96h
	MODRM | BYTE1,			//97h
	MODRM | BYTE1,			//98h
	MODRM | BYTE1,			//99h
	MODRM | BYTE1,			//9Ah
	MODRM | BYTE1,			//9Bh
	MODRM | BYTE1,			//9Ch
	MODRM | BYTE1,			//9Dh
	MODRM | BYTE1,			//9Eh
	MODRM | BYTE1,			//9Fh

	0,						//A0h
	0,						//A1h
	0,						//A2h
	MODRM | DWORD_W,		//A3h
	MODRM | DWORD_W | IMMEDIATE8,//A4h
	MODRM | DWORD_W,		//A5h
	RESERVED,				//A6h
	RESERVED,				//A7h
	0,						//A8h
	0,						//A9h
	0,						//AAh
	MODRM | DWORD_W,		//ABh
	MODRM | DWORD_W | IMMEDIATE8,//ACh
	MODRM | DWORD_W,		//ADh
	GRP | MODRM | DWORD1,	//AEh
	MODRM | DWORD_W,		//AFh

	MODRM | BYTE1,			//B0h
	MODRM | DWORD_W,		//B1h
	MODRM | DWORD1,			//B2h
	MODRM | DWORD_W,		//B3h
	MODRM | DWORD1,			//B4h
	MODRM | DWORD1,			//B5h
	MODRM | DWORD_W,		//B6h
	MODRM | DWORD_W,		//B7h
	RESERVED,				//B8h
	RESERVED,				//B9h
	MODRM | IMMEDIATE8 | DWORD_W | GRP,//BAh
	MODRM | DWORD_W,		//BBh
	MODRM | DWORD_W,		//BCh
	MODRM | DWORD_W,		//BDh
	MODRM | DWORD_W,		//BEh
	MODRM | DWORD_W,		//BFh

	MODRM | BYTE1,			//C0h
	MODRM | DWORD_W,		//C1h
	MODRM | XMM | PS | IMMEDIATE8,	//C2h
	MODRM | DWORD1 | MEM,			//C3h
	MODRM | MMX | WORD1 | IMMEDIATE8,//C4h
	MODRM | MMX | WORD1 | IMMEDIATE8,//C5h
	MODRM | XMM | PS | IMMEDIATE8,//C6h
	MODRM | GRP | Q,		//C7h
	REG | DWORD1,			//C8h
	REG | DWORD1,			//C9h
	REG | DWORD1,			//CAh
	REG | DWORD1,			//CBh
	REG | DWORD1,			//CCh
	REG | DWORD1,			//CDh
	REG | DWORD1,			//CEh
	REG | DWORD1,			//CFh

	RESERVED,				//D0h
	MODRM | MMX | Q,		//D1h
	MODRM | MMX | Q,		//D2h
	MODRM | MMX | Q,		//D3h
	MODRM | MMX | Q,		//D4h
	MODRM | MMX | Q,		//D5h
	RESERVED,				//D6h
	MODRM | MMX | Q | DWORD1,//D7h
	MODRM | MMX | Q,		//D8h
	MODRM | MMX | Q,		//D9h
	MODRM | MMX | Q,		//DAh
	MODRM | MMX | Q,		//DBh
	MODRM | MMX | Q,		//DCh
	MODRM | MMX | Q,		//DDh
	MODRM | MMX | Q,		//DEh
	MODRM | MMX | Q,		//DFh

	MODRM | MMX | Q,		//E0h
	MODRM | MMX | Q,		//E1h
	MODRM | MMX | Q,		//E2h
	MODRM | MMX | Q,		//E3h
	MODRM | MMX | Q,		//E4h
	MODRM | MMX | Q,		//E5h
	RESERVED,				//E6h
	MODRM | MMX | Q | MEM,	//E7h
	MODRM | MMX | Q,		//E8h
	MODRM | MMX | Q,		//E9h
	MODRM | MMX | Q,		//EAh
	MODRM | MMX | Q,		//EBh
	MODRM | MMX | Q,		//ECh
	MODRM | MMX | Q,		//EDh
	MODRM | MMX | Q,		//EEh
	MODRM | MMX | Q,		//EFh

	RESERVED,				//F0h
	MODRM | MMX | Q,		//F1h
	MODRM | MMX | Q,		//F2h
	MODRM | MMX | Q,		//F3h
	MODRM | MMX | Q,		//F4h
	MODRM | MMX | Q,		//F5h
	MODRM | MMX | Q,		//F6h
	MODRM | MMX | Q,		//F7h
	MODRM | MMX | Q,		//F8h
	MODRM | MMX | Q,		//F9h
	MODRM | MMX | Q,		//FAh
	MODRM | MMX | Q,		//FBh
	MODRM | MMX | Q,		//FCh
	MODRM | MMX | Q,		//FDh
	MODRM | MMX | Q,		//EEh
	RESERVED				//FFh
};

//Flags Of One BYTE1 OpCodes ( Table A-2 )
const long OpcodesFlags[256] =
{
	MODRM | BYTE1	| DIRECTION,	//00h
	MODRM | DWORD_W | DIRECTION,	//01h
	MODRM | BYTE1	| DIRECTION,	//02h
	MODRM | DWORD_W | DIRECTION,	//03h
	IMMEDIATE8,						//04h
	IMMEDIATE32_W | EAX | DWORD_W,	//05h
	0,								//06h
	0,								//07h
	MODRM | BYTE1	| DIRECTION,	//08h
	MODRM | DWORD_W	| DIRECTION,	//09h
	MODRM | BYTE1	| DIRECTION,	//0Ah
	MODRM | DWORD_W	| DIRECTION,	//0Bh
	IMMEDIATE8,						//0Ch
	IMMEDIATE32_W | EAX | DWORD_W,	//0Dh
	0,								//0Eh
	TWO,							//0Fh

	MODRM | BYTE1	| DIRECTION,	//10h
	MODRM | DWORD_W | DIRECTION,	//11h
	MODRM | BYTE1	| DIRECTION,	//12h
	MODRM | DWORD_W | DIRECTION,	//13h
	IMMEDIATE8,						//14h
	IMMEDIATE32_W | EAX | DWORD_W,	//15h
	0,								//16h
	0,								//17h
	MODRM | BYTE1	| DIRECTION,	//18h
	MODRM | DWORD_W	| DIRECTION,	//19h
	MODRM | BYTE1	| DIRECTION,	//1Ah
	MODRM | DWORD_W	| DIRECTION,	//1Bh
	IMMEDIATE8,						//1Ch
	IMMEDIATE32_W | EAX | DWORD_W,	//1Dh
	0,								//1Eh
	0,								//1Fh

	MODRM | BYTE1	| DIRECTION,	//20h
	MODRM | DWORD_W | DIRECTION,	//21h
	MODRM | BYTE1	| DIRECTION,	//22h
	MODRM | DWORD_W | DIRECTION,	//23h
	IMMEDIATE8,						//24h
	IMMEDIATE32_W | EAX | DWORD_W,	//25h
	0,								//26h
	0,								//27h
	MODRM | BYTE1	| DIRECTION,	//28h
	MODRM | DWORD_W	| DIRECTION,	//29h
	MODRM | BYTE1	| DIRECTION,	//2Ah
	MODRM | DWORD_W	| DIRECTION,	//2Bh
	IMMEDIATE8,						//2Ch
	IMMEDIATE32_W | EAX | DWORD_W,	//2Dh
	0,								//2Eh
	0,								//2Fh

	MODRM | BYTE1	| DIRECTION,	//30h
	MODRM | DWORD_W | DIRECTION,	//31h
	MODRM | BYTE1	| DIRECTION,	//32h
	MODRM | DWORD_W | DIRECTION,	//33h
	IMMEDIATE8,						//34h
	IMMEDIATE32_W | EAX | DWORD_W,	//35h
	0,								//36h
	0,								//37h
	MODRM | BYTE1	| DIRECTION,	//38h
	MODRM | DWORD_W	| DIRECTION,	//39h
	MODRM | BYTE1	| DIRECTION,	//3Ah
	MODRM | DWORD_W	| DIRECTION,	//3Bh
	IMMEDIATE8,						//3Ch
	IMMEDIATE32_W | EAX | DWORD_W,	//3Dh
	0,								//3Eh
	0,								//3Fh

	DWORD_W | REG,					//40h
	DWORD_W | REG,					//41h
	DWORD_W | REG,					//42h
	DWORD_W | REG,					//43h
	DWORD_W | REG,					//44h
	DWORD_W | REG,					//45h
	DWORD_W | REG,					//46h
	DWORD_W | REG,					//47h
	DWORD_W | REG,					//48h
	DWORD_W | REG,					//49h
	DWORD_W | REG,					//4Ah
	DWORD_W | REG,					//4Bh
	DWORD_W | REG,					//4Ch
	DWORD_W | REG,					//4Dh
	DWORD_W | REG,					//4Eh
	DWORD_W | REG,					//4Fh

	REG | DWORD_W,					//50h
	REG | DWORD_W,					//51h
	REG | DWORD_W,					//52h
	REG | DWORD_W,					//53h
	REG | DWORD_W,					//54h
	REG | DWORD_W,					//55h
	REG | DWORD_W,					//56h
	REG | DWORD_W,					//57h
	REG | DWORD_W,					//58h
	REG | DWORD_W,					//59h
	REG | DWORD_W,					//5Ah
	REG | DWORD_W,					//5Bh
	REG | DWORD_W,					//5Ch
	REG | DWORD_W,					//5Dh
	REG | DWORD_W,					//5Eh
	REG | DWORD_W,					//5Fh

	DWORD_W,						//60h
	DWORD_W,						//61h
	MODRM | A | MEM | DWORD_W,		//62h
	MODRM | WORD1,					//63h
	0,								//64h
	0,								//65h
	0,								//66h
	0,								//67h
	IMMEDIATE32_W,					//68h
	MODRM | DWORD_W | IMMEDIATE32_W,//69h
	IMMEDIATE8,						//6Ah
	MODRM | DWORD_W | IMMEDIATE8,	//6Bh
	ESDI | BYTE1,					//6Ch
	ESDI | DWORD_W,					//6Dh
	DSSI | BYTE1,					//6Eh
	DSSI | DWORD_W,					//6Fh

	OFFSET | BYTE1,					//70h
	OFFSET | BYTE1,					//71h
	OFFSET | BYTE1,					//72h
	OFFSET | BYTE1,					//73h
	OFFSET | BYTE1,					//74h
	OFFSET | BYTE1,					//75h
	OFFSET | BYTE1,					//76h
	OFFSET | BYTE1,					//77h
	OFFSET | BYTE1,					//78h
	OFFSET | BYTE1,					//79h
	OFFSET | BYTE1,					//7Ah
	OFFSET | BYTE1,					//7Bh
	OFFSET | BYTE1,					//7Ch
	OFFSET | BYTE1,					//7Dh
	OFFSET | BYTE1,					//7Eh
	OFFSET | BYTE1,					//7Fh

	GRP | MODRM | IMMEDIATE8 | BYTE1,//80h
	GRP | MODRM | IMMEDIATE32_W | DWORD_W,//81h
	GRP | MODRM | IMMEDIATE8 | BYTE1,//82h
	GRP | MODRM | IMMEDIATE8 | DWORD_W,//83h
	MODRM | BYTE1 | DIRECTION,		//84h
	MODRM | DWORD_W | DIRECTION,	//85h
	MODRM | BYTE1 | DIRECTION,		//86h
	MODRM | DWORD_W | DIRECTION,	//87h
	MODRM | BYTE1	| DIRECTION,	//88h
	MODRM | DWORD_W | DIRECTION,	//89h
	MODRM | BYTE1	| DIRECTION,	//8Ah
	MODRM | DWORD_W | DIRECTION,	//8Bh
	MODRM | SEG | WORD1 | DIRECTION,	//8Ch
	MODRM | MEM | DWORD_W,			//8Dh
	MODRM | SEG | WORD1 | DIRECTION,	//8Eh
	MODRM | DWORD_W,				//8Fh

	0,								//90h
	REG | DWORD_W,					//91h
	REG | DWORD_W,					//92h
	REG | DWORD_W,					//93h
	REG | DWORD_W,					//94h
	REG | DWORD_W,					//95h
	REG | DWORD_W,					//96h
	REG | DWORD_W,					//97h
	DWORD_W,						//98h
	DWORD_W,						//99h
	POINTER,						//9Ah
	0,								//9Bh
	0,								//9Ch
	0,								//9Dh
	0,								//9Eh
	0,								//9Fh

	DISPLACEMENT32_W | BYTE1 | DIRECTION,//A0h
	DISPLACEMENT32_W | DWORD_W | DIRECTION | EAX,//A1h
	DISPLACEMENT32_W | BYTE1 | DIRECTION,//A2h
	DISPLACEMENT32_W | DWORD_W | DIRECTION | EAX,//A3h
	ESDI | DSSI | BYTE1,				//A4h
	ESDI | DSSI | DWORD_W,			//A5h
	ESDI | DSSI | BYTE1,				//A6h
	ESDI | DSSI | DWORD_W,			//A7h
	IMMEDIATE8,						//A8h
	IMMEDIATE32_W | EAX | DWORD_W,	//A9h
	ESDI | BYTE1,					//AAh
	ESDI | DWORD_W | EAX,			//ABh
	DSSI | BYTE1,					//ACh
	DSSI | DWORD_W | EAX,			//ADh
	ESDI | BYTE1,					//AEh
	ESDI | DWORD_W | EAX,			//AFh

	IMMEDIATE8,						//B0h
	IMMEDIATE8,						//B1h
	IMMEDIATE8,						//B2h
	IMMEDIATE8,						//B3h
	IMMEDIATE8,						//B4h
	IMMEDIATE8,						//B5h
	IMMEDIATE8,						//B6h
	IMMEDIATE8,						//B7h
	IMMEDIATE32_W | REG | DWORD_W,	//B8h
	IMMEDIATE32_W | REG | DWORD_W,	//B9h
	IMMEDIATE32_W | REG | DWORD_W,	//BAh
	IMMEDIATE32_W | REG | DWORD_W,	//BBh
	IMMEDIATE32_W | REG | DWORD_W,	//BCh
	IMMEDIATE32_W | REG | DWORD_W,	//BDh
	IMMEDIATE32_W | REG | DWORD_W,	//BEh
	IMMEDIATE32_W | REG | DWORD_W,	//BFh

	GRP | IMMEDIATE8 | MODRM | BYTE1,//C0h
	GRP | IMMEDIATE8 | MODRM | DWORD_W,//C1h
	IMMEDIATE16,					//C2h
	0,								//C3h
	MEM | MODRM | DWORD_W,//C4h
	MEM | MODRM | DWORD_W,//C5h
	GRP | MODRM | IMMEDIATE8 | BYTE1,//C6h
	GRP | MODRM | IMMEDIATE32_W | DWORD_W,//C7h
	IMMEDIATE8 | IMMEDIATE16,		//C8h
	0,								//C9h
	IMMEDIATE16,					//CAh
	0,								//CBh
	0,								//CCh
	IMMEDIATE8,						//CDh
	0,								//CEh
	0,								//CFh

	GRP | MODRM | BYTE1,			//D0h
	GRP | MODRM | DWORD_W,			//D1h
	GRP | MODRM | BYTE1,			//D2h
	GRP | MODRM | DWORD_W,			//D3h
	IMMEDIATE8,						//D4h
	IMMEDIATE8,						//D5h
	RESERVED,						//D6h
	0,								//D7h
	COP | MODRM,					//D8h
	COP | MODRM,					//D9h
	COP | MODRM | DWORD1,			//DAh
	COP | MODRM,					//DBh
	COP | MODRM,					//DCh
	COP | MODRM,					//DDh
	COP | MODRM | WORD1,			//DEh
	COP | MODRM,					//DFh

	OFFSET | BYTE1,					//E0h
	OFFSET | BYTE1,					//E1h
	OFFSET | BYTE1,					//E2h
	OFFSET | BYTE1,					//E3h
	IMMEDIATE8,						//E4h
	IMMEDIATE8 | EAX | DWORD_W,		//E5h
	IMMEDIATE8,						//E6h
	IMMEDIATE8 | EAX | DWORD_W,		//E7h
	OFFSET | DWORD_W,				//E8h
	OFFSET | DWORD_W,				//E9h
	POINTER,						//EAh
	OFFSET | BYTE1,					//EBh
	EAX | DWORD_W,					//ECh
	EAX | DWORD_W,					//EDh
	0,								//EEh
	EAX | DWORD_W,					//EFh

	0,								//F0h
	RESERVED,						//F1h
	0,								//F2h
	0,								//F3h
	0,								//F4h
	0,								//F5h
	GRP | MODRM | BYTE1,				//F6h
	GRP | MODRM | DWORD_W,			//F7h
	0,								//F8h
	0,								//F9h
	0,								//FAh
	0,								//FBh
	0,								//FCh
	0,								//FDh
	GRP | MODRM | BYTE1,				//FEh
	GRP	| MODRM	| DWORD_W			//FFh
};

//Strings For 1 group
char Group1Strings[8][4] = 
{
	"ADD",				//000h
	"OR",				//001h
	"ADC",				//010h
	"SBB",				//011h
	"AND",				//100h
	"SUB",				//101h
	"XOR",				//110h
	"CMP"				//111h
};

//Strings For 2 group
char Group2Strings[8][4] = 
{
	"ROL",				//000h
	"ROR",				//001h
	"RCL",				//010h
	"RCR",				//011h
	"SHL",				//100h
	"SHR",				//101h
	"SAL",				//110h
	"SAR"				//111h
};

//Strings For 3 group
char Group3Strings[8][5] = 
{
	"TEST",				//000h
	"",					//001h
	"NOT",				//010h
	"NEG",				//011h
	"MUL",				//100h
	"IMUL",				//101h
	"DIV",				//110h
	"IDIV"				//111h
};

//Strings For 4 group
char Group4Strings[8][5] = 
{
	"INC",				//000h
	"DEC",				//001h
	"",					//010h
	"",					//011h
	"",					//100h
	"",					//101h
	"",					//110h
	""					//111h
};

//Strings For 5 group
char Group5Strings[8][6] = 
{
	"INC",				//000h
	"DEC",				//001h
	"CALLN",			//010h
	"CALLF",			//011h
	"JMPN",				//100h
	"JMPF",				//101h
	"PUSH",				//110h
	""					//111h
};

//Strings For 6 group
char Group6Strings[8][6] = 
{
	"SLDT",				//000h
	"STR",				//001h
	"LLDT",				//010h
	"LTR",				//011h
	"VERR",				//100h
	"VERW",				//101h
	"",					//110h
	""					//111h
};

//Strings For 7 group
char Group7Strings[8][7] = 
{
	"SGDT",				//000h
	"SIDT",				//001h
	"LGDT",				//010h
	"LIDT",				//011h
	"SMSW",				//100h
	"",					//101h
	"LMSW",				//110h
	"INVLPG"			//111h
};

//Strings For 16 group
char Group16Strings[8][13] = 
{
	"PREFETCH-NTA",		//000h
	"PREFETCH-T0",		//001h
	"PREFETCH-T1",		//010h
	"PREFETCH-T2",		//011h
	"",					//100h
	"",					//101h
	"",					//110h
	""					//111h
};

//Strings For 12 group
char Group12Strings[8][6] = 
{
	"",					//000h
	"",					//001h
	"PSRLW",			//010h
	"",					//011h
	"PSRAW",			//100h
	"",					//101h
	"PSLLW",			//110h
	""					//111h
};

//Strings For 13 group
char Group13Strings[8][13] = 
{
	"",					//000h
	"",					//001h
	"PSRLD",			//010h
	"",					//011h
	"PSRAD",			//100h
	"",					//101h
	"PSLLD",			//110h
	""					//111h
};

//Strings For 14 group
char Group14Strings[8][13] = 
{
	"",					//000h
	"",					//001h
	"PSRLQ",			//010h
	"PSRLDQ",			//011h
	"",					//100h
	"",					//101h
	"PSLLQ",			//110h
	"PSLLDQ"			//111h
};

//Strings For 8 group
char Group8Strings[8][13] = 
{
	"",				//000h
	"",				//001h
	"",				//010h
	"",				//011h
	"BT",			//100h
	"BTS",			//101h
	"BTR",			//110h
	"BTC"			//111h
};

//Strings For D8
char D8[8][6] = 
{
	"FADD",			//000h
	"FMUL",			//001h
	"FCOM",			//010h
	"FCOMP",		//011h
	"FSUB",			//100h
	"FSUBR",		//101h
	"FDIV",			//110h
	"FDIVR"			//111h
};

//Strings For D9
char D9[8][8] = 
{
	"FLD",			//000h
	"",				//001h
	"FST",			//010h
	"FSTP",			//011h
	"FLDENV",		//100h
	"FLDCW",		//101h
	"FNSTENV",		//110h
	"FNSTCW"		//111h
};

//Strings For DA
char DA[8][8] = 
{
	"FIADD",		//000h
	"FIMUL",		//001h
	"FICOM",		//010h
	"FICOMP",		//011h
	"FISUB",		//100h
	"FISUBR",		//101h
	"FIDIV",		//110h
	"FIDIVR"		//111h
};

//Strings For DB
char DB[8][8] = 
{
	"FILD",			//000h
	"FISTTP",		//001h
	"FIST",			//010h
	"FISTP",		//011h
	"",				//100h
	"FLD",			//101h
	"",				//110h
	"FSTP"			//111h
};

//Strings For DC
char DC[8][8] = 
{
	"FADD",			//000h
	"FMUL",			//001h
	"FCOM",			//010h
	"FCOMP",		//011h
	"FSUB",			//100h
	"FSUBR",		//101h
	"FDIV",			//110h
	"FDIVR"			//111h
};

//Strings For DD
char DD[8][8] = 
{
	"FLD",			//000h
	"",				//001h
	"FST",			//010h
	"FSTP",			//011h
	"FRSTOR",		//100h
	"",				//101h
	"FSAVE",		//110h
	"FSTSW"			//111h
};

//Strings For DF
char DF[8][8] = 
{
	"FILD",			//000h
	"FISTTP",		//001h
	"FIST",			//010h
	"FISTP",		//011h
	"FBLD",			//100h
	"FILD",			//101h
	"FBSTP",		//110h
	"FISTP"			//111h
};


//Cop Operands
char CopOperands[8][6] = 
{
	"ST(0)",		//000h
	"ST(1)",		//001h
	"ST(2)",		//010h
	"ST(3)",		//011h
	"ST(4)",		//100h
	"ST(5)",		//101h
	"ST(6)",		//110h
	"ST(7)"			//111h
};


//Strings Of One BYTE1 Opcodes
char OpcodesStrings[256][12] = 
{
//Здесь в строках используем символ % если непосредсвенный операнд печатется не в конце строки, а где нибудь в середине.
//Если он в конце строки, то при его обнаружении просто его печатаем
	"ADD",				//00h
	"ADD",				//01h
	"ADD",				//02h
	"ADD",				//03h
	"ADD AL,",			//04h
	"ADD",				//05h
	"PUSH ES",			//06h
	"POP ES",			//07h
	"OR",				//08h
	"OR",				//09h
	"OR",				//0Ah
	"OR",				//0Bh
	"OR AL,",			//0Ch
	"OR",				//0Dh
	"PUSH CS",			//0Eh
	"",					//0Fh Escape to opcode to 2-BYTE1

	"ADC",				//10h
	"ADC",				//11h
	"ADC",				//12h
	"ADC",				//13h
	"ADC AL,",			//14h
	"ADC",				//15h
	"PUSH SS",			//16h
	"POP SS",			//17h
	"SBB",				//18h
	"SBB",				//19h
	"SBB",				//1Ah
	"SBB",				//1Bh
	"SBB AL,",			//1Ch
	"SBB",				//1Dh
	"PUSH DS",			//1Eh
	"POP DS",			//1Fh

	"AND",				//20h
	"AND",				//21h
	"AND",				//22h
	"AND",				//23h
	"AND AL,",			//24h
	"AND",				//25h
	"",					//26h
	"DAA",				//27h
	"SUB",				//28h
	"SUB",				//29h
	"SUB",				//2Ah
	"SUB",				//2Bh
	"SUB AL,",			//2Ch
	"SUB",				//2Dh
	"",					//2Eh
	"DAS",				//2Fh

	"XOR",				//30h
	"XOR",				//31h
	"XOR",				//32h
	"XOR",				//33h
	"XOR AL,",			//34h
	"XOR",				//35h
	"",					//36h
	"AAA",				//37h
	"CMP",				//38h
	"CMP",				//39h
	"CMP",				//3Ah
	"CMP",				//3Bh
	"CMP AL,",			//3Ch
	"CMP",				//3Dh
	"",					//3Eh
	"AAS",				//3Fh

	"INC ",				//40h
	"INC ",				//41h
	"INC ",				//42h
	"INC ",				//43h
	"INC ",				//44h
	"INC ",				//45h
	"INC ",				//46h
	"INC ",				//47h
	"DEC ",				//48h
	"DEC ",				//49h
	"DEC ",				//4Ah
	"DEC ",				//4Bh
	"DEC ",				//4Ch
	"DEC ",				//4Dh
	"DEC ",				//4Eh
	"DEC ",				//4Fh

	"PUSH ",			//50h
	"PUSH ",			//51h
	"PUSH ",			//52h
	"PUSH ",			//53h
	"PUSH ",			//54h
	"PUSH ",			//55h
	"PUSH ",			//56h
	"PUSH ",			//57h
	"POP ",				//58h
	"POP ",				//59h
	"POP ",				//5Ah
	"POP ",				//5Bh
	"POP  ",			//5Ch
	"POP ",				//5Dh
	"POP ",				//5Eh
	"POP ",				//5Fh

	"PUSHAD",			//60h
	"POPAD",			//61h
	"BOUND",			//62h
	"ARPL",				//63h
	"",					//64h
	"",					//65h
	"",					//66h
	"",					//67h
	"PUSH ",			//68h
	"IMUL",				//69h
	"PUSH ",			//6Ah
	"IMUL",				//6Bh
	"INS %s,DX",		//6Ch
	"INS %s,DX",		//6Dh
	"OUTS DX,%s",		//6Eh
	"OUTS DX,%s",		//6Fh

	"JO",				//70h
	"JNO",				//71h
	"JB",				//72h
	"JNB",				//73h
	"JZ",				//74h
	"JNZ",				//75h
	"JBE",				//76h
	"JNBE",				//77h
	"JS",				//78h
	"JNS",				//79h
	"JP",				//7Ah
	"JNP",				//7Bh
	"JL",				//7Ch
	"JNL",				//7Dh
	"JLE",				//7Eh
	"JNLE",				//7Fh

	"",					//80h
	"",					//81h
	"",					//82h
	"TEST",				//83h
	"TEST",				//84h
	"TEST",				//85h
	"XCHG",				//86h
	"XCHG",				//87h
	"MOV",				//88h
	"MOV",				//89h
	"MOV",				//8Ah
	"MOV",				//8Bh
	"MOV",				//8Ch
	"LEA",				//8Dh
	"MOV",				//8Eh
	"POP",				//8Fh

	"NOP",				//90h
	"XCHG",				//91h
	"XCHG",				//92h
	"XCHG",				//93h
	"XCHG",				//94h
	"XCHG",				//95h
	"XCHG",				//96h
	"XCHG",				//97h
	"CWDE",				//98h
	"CDQ",				//99h
	"CALL FAR",			//9Ah
	"WAIT",				//9Bh
	"PUSHFD",			//9Ch
	"POPFD",			//9Dh
	"SAHF",				//9Eh
	"LAHF",				//9Fh

	"MOV AL,",			//A0h
	"MOV",				//A1h
	"MOV",				//A2h
	"MOV",				//A3h
	"MOVS",				//A4h
	"",					//A5h
	"CMPS",				//A6h
	"",					//A7h
	"TEST AL,",			//A8h
	"TEST",				//A9h
	"STOSB %s,AL",		//AAh
	"",					//ABh
	"LODSB AL,%s",		//ACh
	"",					//ADh
	"SCASB AL,%s",		//AEh
	"",					//AFh

	"MOV AL,",			//B0h
	"MOV CL,",			//B1h
	"MOV DL,",			//B2h
	"MOV BL,",			//B3h
	"MOV AH,",			//B4h
	"MOV CH,",			//B5h
	"MOV DH,",			//B6h
	"MOV BH,",			//B7h
	"MOV",				//B8h
	"MOV",				//B9h
	"MOV",				//BAh
	"MOV",				//BBh
	"MOV",				//BCh
	"MOV",				//BDh
	"MOV",				//BEh
	"MOV",				//BFh

	"",					//C0h
	"",					//C1h
	"RET ",				//C2h
	"RET",				//C3h
	"LES",				//C4h
	"LDS",				//C5h
	"MOV",				//C6h
	"MOV",				//C7h
	"ENTER",			//C8h
	"LEAVE",			//C9h
	"RETF ",			//CAh
	"RETF",				//CBh
	"INT3",				//CCh
	"INT ",				//CDh
	"INTO",				//CEh
	"IRET",				//CFh

	"",					//D0h
	"",					//D1h
	"",					//D2h
	"",					//D3h
	"AAM ",				//D4h
	"AAD ",				//D5h
	"RESERVED",			//D6h
	"XLAT",				//D7h
	"",					//D8h
	"",					//D9h
	"",					//DAh
	"",					//DBh
	"",					//DCh
	"",					//DDh
	"",					//DEh
	"",					//DFh

	"LOOPNZ",			//E0h
	"LOOPZ",			//E1h
	"LOOP",				//E2h
	"JCXZ",				//E3h
	"IN AL,",			//E4h
	"IN",				//E5h
	"OUT %X,AL",		//E6h
	"OUT",				//E7h
	"CALL",				//E8h
	"JMP",				//E9h
	"JMP FAR",			//EAh
	"JMP",				//EBh
	"IN AL,DX",			//ECh
	"IN",				//EDh
	"OUT DX,AL",		//EEh
	"OUT",				//EFh

	"",					//F0h
	"RESERVED",			//F1h
	"REPNE",			//F2h
	"",					//F3h
	"HLT",				//F4h
	"CMC",				//F5h
	"",					//F6h
	"",					//F7h
	"CLC",				//F8h
	"STC",				//F9h
	"CLI",				//FAh
	"STI",				//FBh
	"CLD",				//FCh
	"STD",				//FDh
	"",					//FEh
	""					//FFh
};

//Strings Of two byte Opcodes
char OpcodesStringsTwo[256][11] = 
{
	"",					//00h
	"",					//01h
	"LAR",				//02h
	"LSL",				//03h
	"",					//04h
	"",					//05h
	"CLTS",				//06h
	"",					//07h
	"INVD",				//08h
	"WBINVD",			//09h
	"",					//0Ah
	"UD2",				//0Bh
	"",					//0Ch
	"",					//0Dh
	"",					//0Eh
	"",					//0Fh

	"MOVUPS",			//10h
	"MOVUPS",			//11h
	"MOVLPS",			//12h
	"MOVLPS",			//13h
	"UNPCKLPS",			//14h
	"UNPCKHPS",			//15h
	"MOVHPS",			//16h
	"MOVHPS",			//17h
	"",					//18h
	"",					//19h
	"",					//1Ah
	"",					//1Bh
	"",					//1Ch
	"",					//1Dh
	"",					//1Eh
	"",					//1Fh

	"MOV",				//20h
	"MOV",				//21h
	"MOV",				//22h
	"MOV",				//23h
	"",					//24h
	"",					//25h
	"",					//26h
	"",					//27h
	"MOVAPS",			//28h
	"MOVAPS",			//29h
	"CVTPI2PS",			//2Ah
	"MOVNTPS",			//2Bh
	"CVTTPS2PI",		//2Ch
	"CVTPS2PI",			//2Dh
	"UCOMISS",			//2Eh
	"COMISS",			//2Fh

	"WRMSR",			//30h
	"RDTSC",			//31h
	"RDMSR",			//32h
	"RDPMC",			//33h
	"SYSENTER",			//34h
	"SYSEXIT",			//35h
	"",					//36h
	"",					//37h
	"",					//38h
	"",					//39h
	"",					//3Ah
	"",					//3Bh
	"",					//3Ch
	"",					//3Dh
	"",					//3Eh
	"",					//3Fh

	"CMOVO",			//40h
	"CMOVNO",			//41h
	"CMOVB",			//42h
	"CMOVAE ",			//43h
	"CMOVE",			//44h
	"CMOVNE",			//45h
	"CMOVBE",			//46h
	"CMOVA",			//47h
	"CMOVS",			//48h
	"CMOVNS",			//49h
	"CMOVP",			//4Ah
	"CMOVNP",			//4Bh
	"CMOVL",			//4Ch
	"CMOVNL",			//4Dh
	"CMOVLE",			//4Eh
	"CMOVNLE",			//4Fh

	"MOVMSKPS",			//50h
	"SQRTPS",			//51h
	"RSQRTPS",			//52h
	"RCPPS",			//53h
	"ANDPS",			//54h
	"ANDNPS",			//55h
	"ORPS",				//56h
	"XORPS",			//57h
	"ADDPS",			//58h
	"MULPS",			//59h
	"CVTPS2PD",			//5Ah
	"CVTDQ2PS",			//5Bh
	"SUBPS",			//5Ch
	"MINPS",			//5Dh
	"DIVPS",			//5Eh
	"MAXPS",			//5Fh

	"PUNPCKLBW",		//60h
	"PUNPCKLWD",		//61h
	"PUNPCKLWQ",		//62h
	"PACKSSWB",			//63h
	"PCMPGTB",			//64h
	"PCMPGTW",			//65h
	"PCMPGTD",			//66h
	"PACKUSWB",			//67h
	"PUNPCKHBW ",		//68h
	"PUNPCKHBD",		//69h
	"PUNPCKHBQ",		//6Ah
	"PACKSSDW",			//6Bh
	"",					//6Ch
	"PUNPCKHQD",		//6Dh
	"MOVD",				//6Eh
	"MOVQ",				//6Fh

	"PSHUFW",			//70h
	"",					//71h
	"",					//72h
	"",					//73h
	"PCMPEQB",			//74h
	"PCMPEQW",			//75h
	"PCMPEQD",			//76h
	"EMMS",				//77h
	"",					//78h
	"",					//79h
	"",					//7Ah
	"",					//7Bh
	"",					//7Ch
	"",					//7Dh
	"MOVD",				//7Eh
	"MOVQ",				//7Fh

	"JO",				//80h
	"JNO",				//81h
	"JB",				//82h
	"JAE",				//83h
	"JE",				//84h
	"JNE",				//85h
	"JBE",				//86h
	"JA",				//87h
	"JS",				//88h
	"JNS",				//89h
	"JP",				//8Ah
	"JNP",				//8Bh
	"JL",				//8Ch
	"JNL",				//8Dh
	"JLE",				//8Eh
	"JNLE",				//8Fh

	"SETO",				//90h
	"SETNO",			//91h
	"SETB",				//92h
	"SETAE",			//93h
	"SETE",				//94h
	"SETNE",			//95h
	"SETBE",			//96h
	"SETA",				//97h
	"SETS",				//98h
	"SETNS",			//99h
	"SETP",				//9Ah
	"SETNP",			//9Bh
	"SETL",				//9Ch
	"SETNL",			//9Dh
	"SETLE",			//9Eh
	"SETNLE",			//9Fh

	"PUSH FS",			//A0h
	"POP FS",			//A1h
	"CPUID",			//A2h
	"BT",				//A3h
	"SHLD",				//A4h
	"SHLD",				//A5h
	"",					//A6h
	"",					//A7h
	"PUSH GS",			//A8h
	"POP GS",			//A9h
	"RSM",				//AAh
	"BTS",				//ABh
	"SHRD",				//ACh
	"SHRD",				//ADh
	"",					//AEh
	"IMUL",				//AFh

	"CMPXCHG",			//B0h
	"CMPXCHG",			//B1h
	"LSS",				//B2h
	"BTR",				//B3h
	"LFS",				//B4h
	"LGS",				//B5h
	"MOVZX",			//B6h
	"MOVZX",			//B7h
	"",					//B8h
	"",					//B9h
	"",					//BAh
	"BTC",				//BBh
	"BSF",				//BCh
	"BSR",				//BDh
	"MOVSX",			//BEh
	"MOVSX",			//BFh

	"XADD",				//C0h
	"XADD",				//C1h
	"CMPPS",			//C2h
	"MOVNTI",			//C3h
	"PINSRW",			//C4h
	"PEXTRW",			//C5h
	"SHUFPS",			//C6h
	"",					//C7h
	"BSWAP ",			//C8h
	"BSWAP ",			//C9h
	"BSWAP ",			//CAh
	"BSWAP ",			//CBh
	"BSWAP ",			//CCh
	"BSWAP ",			//CDh
	"BSWAP ",			//CEh
	"BSWAP ",			//CFh

	"",					//D0h
	"PSRLW",			//D1h
	"PSRLD",			//D2h
	"PSRLQ",			//D3h
	"PADDQ",			//D4h
	"PMULLW",			//D5h
	"",					//D6h
	"PMOVMSKB",			//D7h
	"PSUBUSB",			//D8h
	"PSUBUSW",			//D9h
	"PMINUB",			//DAh
	"PAND",				//DBh
	"PADDUSB",			//DCh
	"PADDUSW",			//DDh
	"PMAXUB",			//DEh
	"PANDN",			//DFh

	"PAVGB",			//E0h
	"PSRAW",			//E1h
	"PSRAD",			//E2h
	"PAVGW",			//E3h
	"PMULHUW",			//E4h
	"PMULHW",			//E5h
	"",					//E6h
	"MOVNTQ",			//E7h
	"PSUBSB",			//E8h
	"PSUBSW",			//E9h
	"PMINSW",			//EAh
	"POR",				//EBh
	"PADDSB",			//ECh
	"PADDSW",			//EDh
	"PMAXSW",			//EEh
	"PXOR",				//EFh

	"",					//F0h
	"PSLLW",			//F1h
	"PSLLD",			//F2h
	"PSLLQ",			//F3h
	"PMULUDQ",			//F4h
	"PMADDWD",			//F5h
	"PSADBW",			//F6h
	"MASKMOVQ",			//F7h
	"PSUBB",			//F8h
	"PSUBW",			//F9h
	"PSUBD",			//FAh
	"PSUBQ",			//FBh
	"PADDB",			//FCh
	"PADDW",			//FDh
	"PADDD",			//FEh
	""					//FFh
};