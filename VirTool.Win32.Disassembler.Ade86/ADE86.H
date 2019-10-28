#ifndef	ADE86_H
#define	ADE86_H

#ifndef Byte
#define Byte unsigned char
#endif

#ifndef Word
#define Word unsigned short
#endif

#ifndef Long
#define	Long unsigned long
#endif

#pragma	pack(1)

/* Error codes for assembler and disassembler */

// Memory limit too low
#define	E86_OUT_OF_MEMORY	1

// Need addition memory to produce code
#define E86_ADDITION_MEMORY	2

// Unknown instruction
#define E86_UNKNOWN_INSTRUCTION	3

// Instruction not found
#define E86_ILLEGAL_INSTRUCTION	4

// Invalid instruction operand
#define E86_INVALID_OPERAND	5

// Unexpected characters on line
#define E86_EXTRA_CHARACTERS	6

// Address expression invalid
#define E86_INVALID_ADDRESS	7

// Value out of range
#define E86_OUT_OF_RANGE	8

// Unassembled instruction cross end of code block
#define E86_CROSS_MEMORY	9

// Operands size not match
#define	E86_SIZE_NOT_MATCH	10

// Superfluous prefix
#define	E86_SUPERFLUOUS_PREFIX	11


static const char* ade86_errors[]= {
	"No errors",
	"Out of memory",
	"Need addition memory",
	"Unknown instrution",
	"Illegal instruction",
	"Invalid operand",
	"Extra characters on line",
	"Invalid address",
	"Value out of range",
	"Unassembled instruction cross end of code block",
	"Operand sizes do not match",
	"Superfluous prefix"
};


/* Instruction encoding flags */

#define	C_ADDR1	0x0000001	// Address bytes 1
#define	C_ADDR2	0x0000002	// Address bytes 2
#define	C_ADDRX	0x0000004	// Address bytes 2 or 4
#define	C_REL	0x0000008	// Relative address
#define	C_DATA1	0x0000010	// Data bytes 1
#define	C_DATA2	0x0000020	// Data bytes 2
#define	C_DATAX	0x0000040	// Data bytes 2 or 4
#define	C_ENTER	0x0000030	// "ENTER" instruction data (Data 1 | Data 2)
#define	C_BITS	0x0000080	// Instruction have bit S (signed value)
#define	C_BITW	0x0000100	// Instruction have bit W (force 8 bit size)
#define	C_BITD	0x0000200	// Instruction have bit D (operand exchagne)
#define	C_TBL2	0x0000400	// Instruction use addition table (0F opcode)
#define	C_CC	0x0000800	// Instruction have condition code
#define	C_AS16	0x0001000	// Default 16 bit address
#define	C_DS16	0x0002000	// Default 16 bit data
#define	C_USE16	0x0004000	// Default mode 16 bit
#define	C_SIZEQ	0x0008000	// Operands size equalent (first = second)
#define	C_MODRM	0x0010000	// Instruction have MODRM byte
#define	C_MODEX	0x0020000	// Opcode extension in MODRM byte
#define	C_SIB	0x0040000	// Instruction have SIB byte
#define	C_JMP	0x0100000	// JMP instruction
#define	C_CALL	0x0200000	// CALL instruciton
#define	C_RET	0x0400000	// RET instruction
#define	C_FAR	0x0800000	// Far JMP/CALL


/*******************
 * DBG86 structure *
 *******************/

typedef struct {
/* 00*/	Long	next;		// Offset to next label or 0
/* 04*/	Byte	reserved[8];
/* 0C*/	Long	address;	// Address of label
/* 10*/	Byte	name[1];	// Name of label (variable length array)
} DBG86;


/*******************
 * ASM86 structure *
 *******************/

typedef struct {
/* 00*/	Long	error;		// Error code
/* 04*/	Long 	bin_size;	// Size of code
/* 08*/	Byte* 	bin_output;	// Pointer to assembled code
/* 0C*/	Long	dbg_size;	// Size of symbol table
/* 10*/ DBG86*	dbg_output;	// Pointer to symbol table
/* 14*/ Long	used_memory;	// Total used memory in bytes
/* 18*/ Long	wanted_memory;	// Wanted memory size in bytes
/* 1C*/	Long	nlines;		// Lines counter
/* 20*/	Long	npasses;	// Passes counter
} ASM86;

typedef struct {
/* 00*/	Long	error;		// Error code
/* 04*/	Long 	code_size;	// Size of code
/* 08*/	Byte* 	code_output;	// Pointer to assembled code
/* 0C*/	Long	used_memory;	// Total used memory in bytes
/* 10*/ Long	wanted_memory;	// Wanted memory size in bytes
/* 14*/ Long	nlines;		// Lines counter
/* 18*/ Long	npasses;	// Passes counter
} _ASM86;


/*******************
 * ARG86 structure *
 *******************/

typedef struct {
/* 00*/	Byte	spec;		// Type specified field
/* 01*/	Byte	optype;		// Operand type
} ARG86;

#define	L_RG	0x10		// Opcode filed
#define	L_MRG	0x20		// ModReg field
#define	L_MRM	0x40		// ModRegMem field
#define	L_SEG	0x50		// Opcode filed 000xx000B

#define	T_REG	0x10		// General register
#define	T_CRX	0x20		// CRX register
#define	T_DRX	0x30		// DRX register
#define	T_STX	0x40		// STX register
#define	T_MMX	0x50		// MMX register
#define	T_XMM	0x60		// XMM register
#define	T_SEG	0x70		// Segment register
#define	T_MEM	0x80		// Memory pointer
#define	T_RM	0x90		// Register or Memory
#define	T_REL	0xA0		// Relative address
#define	T_OFFS	0xB0		// Direct offset
#define	T_FAR	0xC0		// Far address
#define	T_FPTR	0xD0		// Far memory poiner
#define	T_IMM	0xE0		// Immediate value
#define	T_TRX	0xF0		// TRX register

#define	S_SCALE	0x01		// Scale factor
#define	S_INDEX	0x02		// Index register
#define	S_BASE	0x04		// Base register
#define	S_DISP	0x08		// Displacement
#define	S_AM16	0x10		// Address mode 16
#define	S_SHORT	0x20		// Short address
#define	S_SIGND	0x08		// Signed value

/*
	Byte	optype		//  0x10 General register
				//  0x20 CRX register
				//  0x30 DRX register
				//  0x40 STX register
				//  0x50 MMX register
				//  0x60 XMM register
				//  0x70 Segment register
				//  0x80 Memory pointer
				//  0x90 Register or register
				//  0xA0 Relative address
				//  0xB0 Direct offset
				//  0xC0 Far address
				//  0xD0 Far memory pointer
				//  0xE0 Immediate value

	Byte	opspec		//  type = 10,20,30,40,50,60
				//   0..7 Register index
				//  type = 80
				//   0x01 Scale factor
				//   0x02 Index register
				//   0x04 Base register
				//   0x08 Displacement
				//   0x10 Address mode 16
				//   0x20 Short address
				//  type = A0,C0
				//   0x10 Address mode 16
				//   0x20 Short address
				//  type = E0
				//   0x01 Value of data 1
				//   0x08 Signed value
*/


/*******************
 * TBL86 structure *
 *******************/

typedef struct {
/* 00*/	Byte	next;		// Offset to next item or
/* 01*/	Byte	opcode;		// Base opcode
/* 02*/	Byte	opmask;		// Opcode search mask
/* 03*/	Byte	grpmf;		// Group index and instruction mode flags
/* 04*/	Long	flags;		// Instruction encoding flags
/* 08*/	Word	op1;		// Instruction operands
/* 0A*/	Word	op2;           
/* 0C*/	Word	op3;
/* 0E*/	Byte	name[1];	// Name of instruction (variable length array)
} TBL86;

/*
	Byte	next		//  0xFF end of table
				//  0xEE Start of addition table

	Byte	opmask		//  0xFF end of table
				//  0xEE Start of addition table
				//  0xFF No special fields
				//  0xFE Bit W
				//  0xFD Bit D
				//  0xFC Bit W & D
				//  0xF8 Register index
				//  0xF0 Condition code
				//  0xE7 Segment register index
				//  0xE6 Segment register index

	Byte	gprmf		//  0..7 Group index
				//  0x20 Instruction for 16bit mode
				//  0x40 Instruction for 32bit mode
 */


/*******************
 * DIS86 structure *
 *******************/

typedef struct {
/* 00*/	Long	error;		// Error detected or 0 if no errors
/* 04*/	Long	ip;		// Instruction pointer
/* 08*/	Byte	size;		// Size of instruction
/* 09*/	Byte	p_66;		// Data override prefix
/* 0A*/	Byte	p_67;		// Address override prefix
/* 0B*/	Byte	p_rep;		// Repeat prefix
/* 0C*/	Byte	p_seg;		// Segment override prefix
/* 0D*/	Byte	p_lock;		// Bus lock prefix
/* 0E*/	Byte	byte0f;		// Previous opcode byte 0x0F
/* 0F*/	Byte	opcode;		// Opcode byte
/* 10*/	Byte	modrm;		// MODRM byte
/* 11*/	Byte	sib;		// SIB byte
/* 12*/	Long	address;	// Address
/* 16*/	Word	selector;	// Selector
/* 18*/	Long	immval;		// Immediate constant
/* 1C*/	Long	flags;		// Instruction flags
/* 20*/	TBL86*	tbl_entry;	// Instruction decoding information
/* 24*/	Word	op1;		// Instruction operands
/* 26*/	Word	op2;
/* 28*/	Word	op3;
} DIS86;



/*******************
 * ADE86 structure *
 *******************/

typedef struct {
/* 00*/	int (__cdecl *assemble)(
		void* outptr,		// Pointer to memory
		void* maxptr,		// Memory limit
		const char* source,	// Pointer to source code
		Long org,		// Code origin
		int bits);		// Bits mode

/* 04*/	int (__cdecl *unassemble)(
		void* binptr,		// Pointer to binary code
		void* maxptr,		// Memory limit
		DIS86* result,		// Result of unassemble
		Long org,		// Code origin or -1(binptr as origin)
		int bits);		// Bits mode

/* 08*/	int (__cdecl *reasemble)(
		void* outptr,		// Pointer to memory
		void* maxptr,		// Memory limit
		DIS86* dis86,		// Pointer to instruction structure
		Long org);		// New origin or -1

/* 0C*/	int (__cdecl *print_dis86)(
		void* outptr,		// Pointer to memory
		void* maxptr,		// Memory limit
		DIS86* disasm,		// Pointer to unassembled instruction
		DBG86* debug);		// Pointer to symbol table
		
/* 10*/	TBL86*	instruction_table;	// Pointer to instruction table
} ADE86;


ADE86* (*fn_ade86_fixup)(void *engine);


#endif	// ADE86_H
