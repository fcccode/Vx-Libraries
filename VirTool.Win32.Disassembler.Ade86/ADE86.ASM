.386
.model flat
.code

PUBLIC ASSEMBLE
PUBLIC UNASSEMBLE
PUBLIC PRINT_DIS86

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; ADE86 HEADER                                                            ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


ADE86_FIXUP:
	CALL	DELTA10
       DELTA10:
	POP	EAX
	ADD	EAX,ADE86_HEADER-DELTA10
	ADD	[EAX],EAX		; ASSEMBLE
	ADD	[EAX+04H],EAX		; UNASSEMBLE
	ADD	[EAX+08H],EAX		; REASSEMBLE
	ADD	[EAX+0CH],EAX		; PRINT_DIS86
	ADD	[EAX+010H],EAX		; INSTRUCTION_TABLE
	RET

ADE86_HEADER:
	DD ASSEMBLE-ADE86_HEADER
	DD UNASSEMBLE-ADE86_HEADER
	DD REASSEMBLE-ADE86_HEADER
	DD PRINT_DIS86-ADE86_HEADER
	DD INSTRUCTION_TABLE-ADE86_HEADER

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; ASSEMBLER                                                               ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


EAT_WHITESPACE:
	LODSB
	MOV	AH,AL			; Save character into AH
	XLATB				; Character type into AL
	TEST	AL,8			; Eat space characters
	JZ	CHARACTER_OK
	JMP	EAT_WHITESPACE
       CHARACTER_OK:
	DEC	ESI			; Set pointer to scanned character
	RET


GET_ID_LENGTH:
	PUSH	ESI
	LODSB
	XLATB
	TEST	AL,1			; Id start with alpha character
	JNZ	TEST_IDCHAR
	POP	ESI
	STC
	RET
       TEST_IDCHAR:
	LODSB
	XLATB
	TEST	AL,3			; Alpha-numeric character
	JNZ	TEST_IDCHAR
	MOV	EAX,ESI 		; Calc length
	POP	ESI
	SUB	EAX,ESI
	DEC	EAX
	CLC
	RET


STRING_INTO_EAX:
	PUSH	ECX
	PUSH	ESI
	XOR	ECX,ECX
       LOAD_EAX:
	LODSB
	MOV	AH,AL
	XLATB
	TEST	AL,3			; Need alpha-numeric
	JZ	TEST_EAX
	SHL	ECX,8
	JC	BAD_EAX_STR		; More than 4 characters
	MOV	CL,AH
	AND	AL,1			; To upper
	SHL	AL,5
	NOT	AL
	AND	CL,AL
	JMP	LOAD_EAX
       TEST_EAX:
	OR	ECX,ECX
	JZ	BAD_EAX_STR
	MOV	EAX,ECX
	POP	ECX			; Not restore ESI
	POP	ECX
	CLC
	RET
       BAD_EAX_STR:
	POP	ESI
	POP	ECX
	STC
	RET


SCAN_PTR_TYPE:
	PUSH	ECX
	PUSH	EDX
	PUSH	EDI
	PUSH	ESI
	CALL	GET_ID_LENGTH		; Check id length
	JC	NOT_PTRTYPE
	MOV	EDX,EAX 		; Save length
	CALL	DELTA9			; Load qualifiers table
       DELTA9:
	POP	ESI
	ADD	ESI,PTRTYPE_QUALIFIER-DELTA9
	XOR	ECX,ECX
       NEXT_PTRTYPE:
	MOVZX	ECX,CL
	ADD	ESI,ECX 		; Move to next item
	LODSB
	OR	AL,AL			; Not found
	JZ	NOT_PTRTYPE
	MOV	CL,AL			; Save item length
	LODSB
	MOV	CH,AL			; Save item value
	CMP	CL,DL			; Compare strings length
	JNE	NEXT_PTRTYPE
	MOV	EDI,[ESP]		; Load source ptr
       TEST_PTRTYPE:
	MOV	AL,[EDI]		; Load source character
	INC	EDI
	MOV	AH,AL
	XLATB
	AND	AL,1			; To upper
	SHL	AL,5
	NOT	AL
	AND	AH,AL
	MOV	AL,[ESI]
	CMP	AH,AL			; Compare characters
	JNE	NEXT_PTRTYPE
	INC	ESI
	DEC	CL			; Test next character
	JNZ	TEST_PTRTYPE
       PTRTYPE_SCANNED:
	POP	EAX			; Not restore ESI
	MOVZX	EAX,CH
	MOV	ESI,EDI
	POP	EDI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_PTRTYPE:
	XOR	EAX,EAX
	POP	ESI
	POP	EDI
	POP	EDX
	POP	ECX
	STC
	RET



SCAN_SIZE_QUALIFIER:
	PUSH	ECX
	PUSH	EDX
	PUSH	EDI
	PUSH	ESI
	CALL	GET_ID_LENGTH		; Check id length
	JC	NOT_QUALIFIER
	MOV	EDX,EAX 		; Save length
	CALL	DELTA8			; Load qualifiers table
       DELTA8:
	POP	ESI
	ADD	ESI,SIZE_QUALIFIER-DELTA8
	XOR	ECX,ECX
       NEXT_QUALIFIER:
	MOVZX	ECX,CL
	ADD	ESI,ECX 		; Move to next item
	LODSB
	OR	AL,AL			; Not found
	JZ	NOT_QUALIFIER
	MOV	CL,AL			; Save item length
	LODSB
	MOV	CH,AL			; Save item value
	CMP	CL,DL			; Compare strings length
	JNE	NEXT_QUALIFIER
	MOV	EDI,[ESP]		; Load source ptr
       TEST_QUALIFIER:
	MOV	AL,[EDI]		; Load source character
	INC	EDI
	MOV	AH,AL
	XLATB
	AND	AL,1			; To upper
	SHL	AL,5
	NOT	AL
	AND	AH,AL
	MOV	AL,[ESI]
	CMP	AH,AL			; Compare characters
	JNE	NEXT_QUALIFIER
	INC	ESI
	DEC	CL			; Test next character
	JNZ	TEST_QUALIFIER
       QUALIFIER_SCANNED:
	POP	EAX			; Not restore ESI
	MOVZX	EAX,CH
	MOV	ESI,EDI
	POP	EDI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_QUALIFIER:
	XOR	EAX,EAX
	POP	ESI
	POP	EDI
	POP	EDX
	POP	ECX
	STC
	RET


SCAN_EXPRESSION:
	PUSH	ECX
	PUSH	ESI
	XOR	EAX,EAX
	MOV	ECX,EAX
	MOV	AL,[ESI]
	CMP	AL,'+'			; Positive value
	SETE	CL			; Eat sign
	ADD	ESI,ECX
	CMP	AL,'-'			; Negative value
	SETE	CL			; Eat sign
	ADD	ESI,ECX
	MOV	AL,[ESI]
	XLATB
	TEST	AL,2			; Test digit type
	JZ	NOT_EXPRESSION
	XOR	ECX,ECX 		; Clear value
       EAT_ZEROIES:
	LODSB				; Find first non-zero digit
	CMP	AL,'0'
	JE	EAT_ZEROIES
	DEC	ESI
       LOAD_VALUE:
	LODSB
	MOV	AH,AL
	XLATB
	TEST	AL,6			; Test hex-digit character
	JZ	VALUE_CONVERTED
	AND	AL,1			; To upper case
	SHL	AL,5
	NOT	AL
	AND	AH,AL
	SUB	AH,30H			; value of 0..9 characters
	CMP	AH,9
	JBE	DIGIT_OK
	SUB	AH,7			; value of A..F characters
       DIGIT_OK:
	SHL	ECX,4			; Store 4bit value
	JC	OUT_OF_RANGE
	OR	CL,AH
	JMP	LOAD_VALUE
       VALUE_CONVERTED:
	CMP	AH,'H'
	JE	H_SUFFIX
	CMP	AH,'h'
	JE	H_SUFFIX
	DEC	ESI
       H_SUFFIX:
	MOV	[EBP-020H],ECX		; Save result
	MOV	EAX,[ESP]
	MOV	AL,[EAX]
	CMP	AL,'-'			; Test negative value
	JNE	SET_VALUE_SIZE
	MOV	EAX,ECX 		; Change result sign
	NEG	EAX
	MOV	[EBP-020H],EAX
       SET_VALUE_SIZE:
	MOV	EAX,ECX 		; Calc value size in bytes
	MOV	CH,1
	TEST	EAX,0FFFFFF00H
	SETNZ	CL
	SHL	CH,CL
	TEST	EAX,0FFFF0000H
	SETNZ	CL
	SHL	CH,CL
	MOVZX	EAX,CH
       EXPRESSION_OK:
	POP	ECX			; Not restore ESI
	POP	ECX
	CLC
	RET
       NOT_EXPRESSION:
	POP	ESI
	POP	ECX
	STC
	RET


SCAN_REGISTER:
	CALL	EAT_WHITESPACE
	PUSH	ECX
	PUSH	EDX
	PUSH	ESI
	CALL	STRING_INTO_EAX 	; Register name into EAX
	JC	NOT_REGISTER
	CALL	DELTA7			; Load registers table
       DELTA7:
	POP	ESI
	ADD	ESI,GENERAL_REGISTER-DELTA7
	MOV	EDX,EAX 		; Save name into EDX
	TEST	EAX,0FFFF0000H		; 3-characters register
	JNZ	REGISTER_3C
       TEST_R8:
	MOV	CX,1100H		; Test 8bit register
	XCHG	DH,DL
       CMPREG8:
	LODSW
	CMP	AX,DX
	JE	REG_SCANNED
	INC	CL
	CMP	CL,8
	JB	CMPREG8
       TEST_R16:
	MOV	CX,1200H		; Test 16bit register
       CMPREG16:
	LODSW
	CMP	AX,DX
	JE	REG_SCANNED
	INC	CL
	CMP	CL,8
	JB	CMPREG16
       TEST_SEG:
	MOV	CX,7200H		; Test segment register
       CMPREGSG:
	LODSW
	CMP	AX,DX
	JE	REG_SCANNED
	INC	CL
	CMP	CL,6
	JB	CMPREGSG
	JMP	NOT_REGISTER
       REGISTER_3C:
	ROR	EAX,16
	CMP	AH,0			; Something in 4-character
	JNZ	NOT_REGISTER
	MOV	CX,1400H		; Test 32bit register if name
	CMP	AL,'E'			;  start with 'E' character
	JNE	REGISTER_3CX		;  else test CRx,DRx,STx,MMx.
	ADD	ESI,10H
	XCHG	DH,DL
       CMPREG32:
	LODSW
	CMP	AX,DX
	JE	REG_SCANNED
	INC	CL
	CMP	CL,8
	JB	CMPREG32
	JMP	NOT_REGISTER
       REGISTER_3CX:
	MOV	EAX,EDX
	MOV	CL,DL			; Get register index
	SUB	CL,30H
	JC	NOT_REGISTER
	CMP	CL,7			; Bad index, need 0..7
	JG	NOT_REGISTER
	SHR	EAX,8
	MOV	CH,24H			; Test CRx register
	CMP	AX,04352H		; 'CR'
	JE	REG_SCANNED
	MOV	CH,34H			; Test DRx register
	CMP	AX,04452H		; 'DR'
	JE	REG_SCANNED
	MOV	CH,44H			; Test STx register
	CMP	AX,05354H		; 'ST'
	JE	REG_SCANNED
	MOV	CH,58H			; Test MMx register
	CMP	AX,04D4DH		; 'MM'
	JE	REG_SCANNED
	MOV	CH,0F4H 		; Test TRx register
	CMP	AX,05452H		; 'TR'
	JE	REG_SCANNED
	JMP	NOT_REGISTER
       REG_SCANNED:
	POP	ESI
	CALL	GET_ID_LENGTH
	ADD	ESI,EAX
	MOVZX	EAX,CX
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_REGISTER:
	POP	ESI
	POP	EDX
	POP	ECX
	STC
	RET


SCAN_CONDITION_CODE:
	PUSH	ECX
	PUSH	EDX
	PUSH	ESI
	CALL	STRING_INTO_EAX 	; Condition code into EAX
	JC	NOT_CC
	MOV	EDX,EAX 		; Save result into EDX
	CALL	DELTA6			; Load codes table
       DELTA6:
	POP	ESI
	ADD	ESI,CONDITION_CODE-DELTA6
       TEST_CC:
	LODSB
	OR	AL,AL			; Not found
	JZ	NOT_CC
	MOV	CL,AL			; Save item length
	LODSB
	MOV	CH,AL			; Save item value
	XOR	EAX,EAX 		; Load item text into EAX
       LOAD_CC:
	SHL	EAX,8
	LODSB
	DEC	CL
	JNZ	LOAD_CC
	CMP	EAX,EDX 		; Compare strings
	JNE	TEST_CC 		; Test next item
	MOVZX	EAX,CH
	POP	EDX			; Not restore ESI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_CC:
	POP	ESI
	POP	EDX
	POP	ECX
	STC
	RET


SCAN_MEMPTR:
	PUSH	ECX
	PUSH	EDX
	PUSH	ESI
	XOR	ECX,ECX
	MOV	[EBP-020H],ECX		; Clear value for eBP+00 addressing
	MOV	EAX,[EBP-044H]
	TEST	EAX,0300000H		; Type qualifier only for jmp/call
	JZ	NOT_PTR_QUALIFIER
	CALL	SCAN_PTR_TYPE
	JC	NOT_PTR_QUALIFIER
	MOV	DL,AL
	CALL	EAT_WHITESPACE
	CMP	AH,'['
	JNE	NOT_MEMPTR
	INC	ESI			; Eat '['
	TEST	DL,06H			; Test far/near qualifier
	JZ	INVALID_OPERAND
	MOV	EAX,[EBP-018H]		; Calc pointer size 2/4
	SHR	EAX,3
	MOV	CH,080H 		; Near same as word/dword qualifier
	OR	CH,AL
	CMP	DL,2
	JE	PTR_EXPRESSION
	SHR	AL,1			; Calc pointer size 4/6
	MOV	CH,AL
	OR	CH,DL
	OR	CH,0D0H 		; Special type for far qualifier
	JMP	PTR_EXPRESSION
       NOT_PTR_QUALIFIER:
	CALL	EAT_WHITESPACE
	CMP	AH,'['			; Expression start with '['
	JNE	NOT_MEMPTR
	INC	ESI			; Eat '[' character
	CALL	SCAN_SIZE_QUALIFIER	; May be size qualifier
	MOV	CH,AL			; Save memptr size
	OR	CH,80H			; Memory pointer argument
       PTR_EXPRESSION:
	CALL	SCAN_REGISTER		; Registers in expression
	JNC	CHECK_PTR_MODE
	CALL	SCAN_EXPRESSION 	; Displacement only
	JC	INVALID_ADDRESS
	MOV	EDX,[EBP-020H]		; Save offset value
	MOV	[EBP-04EH],EDX
	OR	CL,08H			; Displacement falg
	CMP	AL,2
	JG	D32_OFFSET
	OR	CL,010H 		; Set address mode 16 flag
	MOV	AL,6
	OR	[EBP-050H],AL		; Set 06 in MOD RegMem field
	MOV	EAX,2
	OR	[EBP-044H],EAX		; Set displacement 16bit
	JMP	NO_DISPLACEMENT
       D32_OFFSET:
	MOV	AL,5
	OR	[EBP-050H],AL		; Set 05(EBP) in MOD RegMem field
	MOV	EAX,4
	OR	[EBP-044H],EAX		; Set displacement 32bit
	JMP	NO_DISPLACEMENT
       CHECK_PTR_MODE:
	XOR	EDX,EDX 		; DL base, DH index
	CMP	AH,12H			; Mode depend on register size
	JE	PTR_MODE16		; 16bit address mode
	CMP	AH,14H
	JE	PTR_MODE32		; 32bit address mode
	JMP	INVALID_ADDRESS
       PTR_MODE16:
	OR	CL,010H 		; Address mode16 flag
	MOV	DL,AL			; Base register found
	OR	CL,04H
	CALL	EAT_WHITESPACE
	CMP	AH,'+'			; Have index reigster?
	JNE	CHECK_DISP16
	INC	ESI			; Eat '+' character
	CALL	SCAN_REGISTER		; Index register or displacement
	JC	SCAN_DISP16
	MOV	DH,AL			; Index register found
	OR	CL,02H
	CMP	AH,12H			; Need 16bit register
	JNE	INVALID_ADDRESS
       CHECK_DISP16:
	CALL	EAT_WHITESPACE
	CMP	AH,'+'			; Positive displacement
	JE	SCAN_DISP16
	CMP	AH,'-'			; Negative displacement
	JE	SCAN_DISP16
	JMP	PTR_MODE16_OK
       SCAN_DISP16:
	CALL	SCAN_EXPRESSION 	; Get displacement value
	JC	INVALID_ADDRESS
	OR	CL,08H			; Displacement found
	CMP	AL,2			; Value great than 16bit
	JG	INVALID_ADDRESS
	CMP	AL,1
	JG	PTR_MODE16_OK
	OR	CL,20H			; Short address flag
       PTR_MODE16_OK:
	MOV	AL,CL
	AND	AL,08H			; Get displacement flag
	OR	AL,DL			; Get base register
	OR	AL,DH			; Get index register
	CMP	AL,05H			; BP as base need +00 displacement
	SETE	AH
	SHL	AH,3
	OR	CL,AH			; Set disp flag if BP+00
	SHL	AH,2
	OR	CL,AH			; Set short address flag if BP+00
	MOV	AL,0
	CMP	DX,0603H		; BX+SI 0
	JE	SET_RM16
	INC	AL
	CMP	DX,0703H		; BX+DI 1
	JE	SET_RM16
	INC	AL
	CMP	DX,0605H		; BP+SI 2
	JE	SET_RM16
	INC	AL
	CMP	DX,0705H		; BP+DI 3
	JE	SET_RM16
	INC	AL
	CMP	DX,0006H		; SI    4
	JE	SET_RM16
	INC	AL
	CMP	DX,0007H		; DI    5
	JE	SET_RM16
	INC	AL
	CMP	DX,0005H		; BP    6
	JE	SET_RM16
	INC	AL
	CMP	DX,0003H		; BX    7
	JE	SET_RM16
	JMP	INVALID_ADDRESS
       SET_RM16:
	OR	[EBP-050H],AL		; Set MOD RegMem fieLd
	JMP	MEMPTR_OK
       PTR_MODE32:
	MOV	EDX,EAX 		; Save SCAN_REGISTER result
	CALL	EAT_WHITESPACE
	CMP	AH,'*'			; Void base register
	JNE	BASE32
	MOV	EAX,EDX
	MOV	DL,0			; Clear base
	JMP	INDEX32
       BASE32:
	MOV	EAX,EDX
	OR	CL,04H			; Base register flag
	MOV	DL,AL			; Save base register
	MOV	DH,0			; Clear index
	MOV	AL,[ESI]
	CMP	AL,'+'
	JNE	CHECK_DISP32
	INC	ESI			; Eat '+' character
	CALL	SCAN_REGISTER
	JC	SCAN_DISP32
       INDEX32:
	CMP	AH,14H			; Need 32bit register
	JNE	INVALID_ADDRESS
	CMP	AL,4			; ESP not allowed as index
	JE	INVALID_ADDRESS
	MOV	DH,AL			; Save index register
	OR	CL,02H			; Index register flag
	CALL	EAT_WHITESPACE
	CMP	AH,'*'			; Check scale factor
	JNE	CHECK_DISP32
	INC	ESI			; Eat '*' character
	CALL	EAT_WHITESPACE
	MOV	AL,00H
	CMP	AH,'1'			; Test scale factor
	JE	SCALE32
	MOV	AL,040H
	CMP	AH,'2'
	JE	SCALE32
	MOV	AL,080H
	CMP	AH,'4'
	JE	SCALE32
	MOV	AL,0C0H
	CMP	AH,'8'
	JE	SCALE32
	JMP	INVALID_ADDRESS
       SCALE32:
	CMP	AH,'2'
	SETE	AH
	SHL	AH,7
	OR	CL,AH			; Scale allow optimization
	INC	ESI			; Eat digit
	OR	[EBP-04FH],AL		; Set scale in SIB byte
       CHECK_DISP32:
	CALL	EAT_WHITESPACE
	CMP	AH,'+'
	JE	SCAN_DISP32
	CMP	AH,'-'
	JE	SCAN_DISP32
	JMP	PTR_MODE32_OK
       SCAN_DISP32:
	CALL	SCAN_EXPRESSION
	JC	INVALID_ADDRESS
	OR	CL,08H			; Displacement flag
	CMP	AL,4
	JG	INVALID_ADDRESS 	; Address great than 2^32
	CMP	AL,1
	JG	PTR_MODE32_OK
	OR	CL,20H			; Short address flag
       PTR_MODE32_OK:
	MOV	AL,CL
	AND	AL,08H			; Get displacement flag
	OR	AL,DL			; Get base register
	CMP	AL,05H			; EBP as base register
	SETE	AH
	SHL	AH,3			; Displacement flag if EBP+00
	OR	CL,AH
	SHL	AH,2
	OR	CL,AH			; Short address flag if EBP+00
	TEST	CL,03H			; Test index or scale flags
	JNZ	SET_SIB
	CMP	DL,4			; ESP as base only in SIB addressing
	JE	SET_SIB
	OR	[EBP-050H],DL		; Set base register in MODRM
	JMP	MEMPTR_OK
       SET_SIB:
	MOV	EAX,040000H
	OR	[EBP-044H],EAX		; Set SIB flag
	MOV	AL,4
	OR	[EBP-050H],AL		; 04H(ESP) in MODRM field
	MOV	AL,DH			; Set index register
	SHL	AL,3
	OR	AL,DL			; Set base register
	OR	[EBP-04FH],AL		; Set SIB byte
	TEST	CL,02H			; Test void index
	SETZ	AL
	SHL	AL,5
	OR	[EBP-04FH],AL		; 04(ESP) if no index register
	TEST	CL,04H			; Test void base
	JNZ	MEMPTR_OK
	TEST	CL,080H 		; Optimization index*2 => index+index
	JNZ	INDEX_OPTIMIZE
	MOV	AL,5
	OR	[EBP-04FH],AL		; Set SIB RegMem field 05(disp32)
	MOV	EAX,4
	OR	[EBP-044H],EAX		; Set displacement 32bit
	JMP	MEMPTR_OK
       INDEX_OPTIMIZE:
	MOV	AL,[EBP-04FH]		; Load SIB byte
	AND	AL,03FH 		; Clear scale field
	OR	AL,DH
	MOV	[EBP-04FH],AL		; Set index+index in SIB
       MEMPTR_OK:
	TEST	CL,08H
	JZ	NO_DISPLACEMENT
	PUSH	ECX
	MOV	EAX,[EBP-020H]		; Set displacement value
	MOV	[EBP-04EH],EAX
	MOV	AL,080H 		; Displacement 16bit
	MOV	AH,CL
	TEST	AH,020H 		; Test short address flag
	SETNZ	CL
	SHR	AL,CL
	OR	[EBP-050H],AL		; Set MOD filed
	MOV	EDX,4
	TEST	AH,010H 		; Test address mode 16
	SETNZ	CL
	SHR	EDX,CL
	TEST	AH,020H 		; Test short address flag
	JZ	SET_DISP_FLAG
	MOV	DL,1
       SET_DISP_FLAG:
	OR	[EBP-044H],EDX		; Set 8/16/32bit displacement
	POP	ECX
       NO_DISPLACEMENT:
	CALL	EAT_WHITESPACE
	CMP	AH,']'
	JNE	INVALID_ADDRESS
	INC	ESI
	MOVZX	EAX,CX
	POP	EDX			; Not need to restore ESI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_MEMPTR:
	POP	ESI
	POP	EDX
	POP	ECX
	STC
	RET


SCAN_ADDRESS:
	PUSH	ECX
	PUSH	EDX
	PUSH	EDI
	PUSH	ESI
	XOR	ECX,ECX
	CALL	SCAN_EXPRESSION
	JC	NOT_ADDRESS
	MOV	CH,AL			; Address size
	CALL	EAT_WHITESPACE
	CMP	AH,':'
	JE	FAR_ADDRESS
	MOV	EDX,[EBP-018H]		; Get current bits mode
	SHR	EDX,3			; Calc default address size
	MOV	EAX,[EBP-044H]		; Load instruction flags
	TEST	EAX,0200000H		; CALL instruction 3-5 byte
	JNZ	CALL_NEAR
	TEST	AX,0800H		; Jcc near 4-6 byte
	SETNZ	CL
	ADD	DL,CL
	MOV	ECX,[EBP-014H]		; Current origin
	MOV	EAX,[EBP-020H]		; Destanation address
	MOV	EDI,EAX
	MOV	AL,[EBP-051H]		; JCXZ instruction in various modes
	CMP	AL,0E3H 		;  have different length, so range
	JNE	JCXZ_TESTED		;  of forward jump 81 or 82 and
					;  backward jump 7E or 7D.
	MOV	EAX,[EBP-040H]		; Load instruction mode from TBL86
	MOV	AL,[EAX+03H]
	MOV	AH,[EBP-018H]		; Calc current bits mode
	SHL	AH,1			; Mask for instruction mode
	TEST	AH,AL			; Test modes eqalense
	JNZ	JCXZ_TESTED
	MOV	EDX,3			; Instruction length 3
	MOV	EAX,EDI
	SUB	EAX,ECX
	JS	JCXZ_BACK
	CMP	EAX,082H		; Short forward jump 7E+3 (82)
	JBE	JMP_SHORT
       JCXZ_BACK:
	MOV	EDI,EAX
	NEG	EDI
	CMP	EDI,07DH		; Short backward jump 80-3 (7D)
	JLE	JMP_SHORT
	JMP	OUT_OF_RANGE
       JCXZ_TESTED:
	MOV	EAX,EDI
	SUB	EAX,ECX
	JS	JMP_BACK
	CMP	EAX,081H		; Short forward jump 7E+2 (81)
	JG	JMP_NEAR		;  max range + instruction length
	MOV	EDX,2			; Instruction length 2
	JMP	JMP_SHORT
       JMP_BACK:
	MOV	EDI,EAX
	NEG	EDI
	CMP	EDI,07EH		; Short backward jump 80-2 (7E)
	JG	JMP_NEAR		;  max range - instruction length
	MOV	EDX,2			; Instruction length 2
	JMP	JMP_SHORT
       JMP_NEAR:
	INC	EDX			; Jmp near 3-5 or Jcc near 4-6 byte
	SUB	EAX,EDX
	MOV	[EBP-04Eh],EAX		; Set address
	MOV	EAX,[EBP-018H]		; Get current bits mode
	SHR	EAX,3			; Calc default address size
	OR	[EBP-044H],EAX		; Near address instruction flag
	JMP	DEST_OK
       JMP_SHORT:
	SUB	EAX,EDX 		; Jmp short 2 byte or jcxz 3
	MOV	[EBP-04Eh],EAX		; Set address
	MOV	EAX,1
	OR	[EBP-044H],EAX		; Short address instruction flag
	JMP	DEST_OK
       CALL_NEAR:
	INC	EDX			; Call near 3-5 byte
	MOV	ECX,[EBP-014H]		; Current origin
	MOV	EAX,[EBP-020H]		; Destanation address
	SUB	EAX,ECX
	SUB	EAX,EDX
	MOV	[EBP-04EH],EAX		; Save address constant
	MOV	EAX,[EBP-018H]		; Get current bits mode
	SHR	EAX,3			; Calc default address size
	OR	[EBP-044H],EAX		; Near address instruction flag
       DEST_OK:
	MOV	CL,0
	MOV	CH,0A0H 		; Relative address
	OR	CH,AL
	CMP	AL,2
	JG	ADDRESS_OK
	OR	CL,10H			; Address mode16 flag
	CMP	AL,1
	JG	ADDRESS_OK
	OR	CL,20H			; Short address flag
	JMP	ADDRESS_OK
       FAR_ADDRESS:
	INC	ESI			; Eat ':' character
	MOV	CH,0C0H 		; FAR address (Selector size not used)
	CMP	AL,2
	JG	INVALID_ADDRESS
	MOV	EAX,[EBP-020H]
	MOV	[EBP-04AH],AX		; Save selector constant
	CALL	SCAN_EXPRESSION
	JC	INVALID_ADDRESS
	MOV	EDX,[EBP-020H]		; Save address constant
	MOV	[EBP-04EH],EDX
	MOV	EAX,[EBP-018H]		; Align on default address size
	SHR	EAX,3
	OR	CH,AL			; Set only offset size
	OR	[EBP-044H],EAX		; Set far address instruction flag
	CMP	AL,2
	JG	ADDRESS_OK
	OR	CL,10H			; Address mode16 flag
       ADDRESS_OK:
	MOVZX	EAX,CX
	POP	EDI			; Not restore ESI
	POP	EDI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_ADDRESS:
	POP	ESI
	POP	EDI
	POP	EDX
	POP	ECX
	STC
	RET


SCAN_OPERAND:
	PUSH	EDI
	LEA	EDI,[EBP-03CH]		; Pointer to operands array
	LEA	EDI,[EDI+ECX*2] 	; Load current operand offset
	CALL	SCAN_REGISTER
	JNC	OPERAND_OK
	CALL	SCAN_MEMPTR
	JNC	OPERAND_OK
	MOV	EAX,[EBP-040H]		; Load TBL86 flags
	MOV	EAX,[EAX+04H]
	TEST	AL,8			; If operand relative address
	JNZ	OP_ADDRESS		;  need to call SCAN_ADDRESS.
	AND	AL,70H
	CMP	AL,30H			; ENTER instruction operands
	JE	OP_ENTER
	CALL	SCAN_EXPRESSION
	JNC	OP_IMM
	JMP	NOT_OPERAND
       OP_ENTER:
	CALL	SCAN_EXPRESSION
	JC	INVALID_OPERAND
	CMP	AL,2
	JG	OUT_OF_RANGE
	MOV	EAX,[EBP-020H]		; Save local size constant
	MOV	[EBP-048H],EAX
	CALL	EAT_WHITESPACE
	CMP	AH,','
	JNE	INVALID_OPERAND
	INC	ESI			; Eat ','
	CALL	EAT_WHITESPACE
	CALL	SCAN_EXPRESSION
	JC	INVALID_OPERAND
	CMP	AL,1
	JG	OUT_OF_RANGE
	MOV	AL,[EBP-020H]
	MOV	[EBP-046H],AL		; Save recursion constant
	MOV	AX,0E200H		; Save operand 1
	STOSW
	MOV	AX,0E100H		; Saving in OPERAND_OK action
	JMP	OPERAND_OK
       OP_IMM:
	MOV	AH,AL
	OR	AH,0E0H 		; Imm argument
	MOV	AL,0
	PUSH	EAX
	MOV	EAX,[EBP-20H]
	MOV	[EBP-048H],EAX		; Save address constant
	POP	EAX
	JMP	OPERAND_OK
       OP_ADDRESS:
	CALL	SCAN_ADDRESS
	JC	NOT_OPERAND
	JMP	OPERAND_OK
       NOT_OPERAND:
	POP	EDI
	STC
	RET
       OPERAND_OK:
	STOSW
	POP	EDI
	CLC
	RET


COMPARE_OPERANDS:
	PUSH	EDX
	MOV	DH,AH			; Compare types
	MOV	DL,BH
	AND	DX,00F0FH		; DH size, DL scanned size
	AND	AH,0F0H 		; AH type, BH scanned type
	AND	BH,0F0H
	CMP	AH,090H 		; Memory or register
	JE	CMP_OPRM
	CMP	AH,0B0H 		; Direct offset
	JE	CMP_OFFS
	CMP	AH,0D0H 		; Far memory pointer
	JE	CMP_MPTR
	CMP	AH,BH
	JNE	OPERAND_NE
	CMP	AH,010H 		; General registers
	JE	CMP_OPREG
	CMP	AH,070H 		; Segment register
	JE	CMP_OPREG
	CMP	AH,080H 		; Memory pointer
	JE	CMP_OPMEM
	CMP	AH,0A0H 		; Relative address
	JE	CMP_OPREL
	CMP	AH,0C0H 		; Far address
	JE	CMP_OPFAR
	CMP	AH,0E0H 		; Imm constant
	JE	CMP_OPIMM
	CMP	DH,DL			; All other types,simple compare size
	JE	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_MPTR:
	JMP	OPERAND_EQ
       CMP_OFFS:
	CMP	BH,080H 		; Need memory pointer
	JNE	OPERAND_NE
	AND	BL,0FH
	CMP	BL,08H
	JNE	OPERAND_NE
	JMP	OPERAND_EQ
       CMP_OPRM:
	CMP	BH,010H 		; Test as register
	JE	CMP_OPREG
	CMP	BH,080H 		; Test as memory
	JE	CMP_OPMEM
	JMP	OPERAND_NE
       CMP_OPMEM:
	OR	DH,DH			; Size default for current mode
	JE	CMP_DEFMEM
	CMP	DH,DL			; Test hard specified size
	JE	OPERAND_EQ
	OR	DL,DL			; Allow use hard specified size
	JZ	OPERAND_EQ		;  as default for memory operation.
	JMP	OPERAND_NE
       CMP_DEFMEM:
	OR	DL,DL			; Second operand set size
	JE	OPERAND_EQ
	OR	CH,CH			; Single memory operation
	JZ	OPERAND_EQ
	MOV	CL,CH
	AND	CH,0FH
	AND	CL,0F0H
	CMP	CL,0E0H 		; Memory pointer and constant
	JE	CMP_MEMIMM
	CMP	CH,DL			; Test operand sizes match
	JE	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_MEMIMM:
	CMP	CH,DL
	JG	OPERAND_NE
	JMP	OPERAND_EQ
       CMP_OPREG:
	MOV	AH,AL
	TEST	AH,8			; Pseudo register
	JZ	NO_PSEUDO
	AND	AH,7
	CMP	AH,BL
	JNE	OPERAND_NE
       NO_PSEUDO:
	OR	DH,DH			; Default register size
	JZ	CMP_DEFOP
	CMP	DH,DL			; Compare hard specified size
	JE	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_OPREL:
	OR	DH,DH			; Default address size
	JZ	CMP_DEFVAL
	CMP	DH,DL			; Compare hard specified size
	JGE	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_OPFAR:
	JMP	OPERAND_EQ
       CMP_OPIMM:
	OR	DH,DH			; Default imm size
	JZ	CMP_DEFVAL
	CMP	DL,DH			; Compare hard specified size
	JBE	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_DEFOP:
	CMP	DL,2			; Need 16bit or 32bit operand
	JE	OPERAND_EQ
	CMP	DL,4
	JE	OPERAND_EQ
	CMP	DL,1			; Test 8bit operand with bit W
	JNE	OPERAND_NE
	MOV	EAX,[EBP-044H]		; Test bit W flag
	TEST	EAX,0100H
	JNZ	OPERAND_EQ
	JMP	OPERAND_NE
       CMP_DEFVAL:
	CMP	DL,4			; Need 16bit ot 32bit value
	JBE	OPERAND_EQ
	JMP	OPERAND_NE
       OPERAND_EQ:
	POP	EDX
	CLC
	RET
       OPERAND_NE:
	POP	EDX
	STC
	RET


SCAN_INSTRUCTION:
	PUSH	ECX
	PUSH	EDX
	PUSH	EDI
	PUSH	ESI
	MOV	EDX,EAX
	CALL	GET_ID_LENGTH		; Get identifier length
	MOV	ESI,EDX 		; Instruction table into ESI
	MOV	EDX,EAX 		; Save length into EDX
	MOV	[EBP-040H],ESI		; Save pointer to current TBL86 item
       TEST_INSTRUCTION:
	MOV	AL,[ESI]
	CMP	AL,0FFH 		; End of table
	JE	NOT_INSTRUCTION
	CMP	AL,0EEH 		; Start of addition table
	JE	ADDITION_TABLE
	LEA	ESI,[ESI+0EH]		; Instruction name
	CALL	GET_ID_LENGTH
	CMP	EAX,EDX 		; Length mismatch
	JG	NEXT_INSTRUCTION
	MOV	ECX,EAX
	MOV	EDI,[ESP]		; Pointer to id
	MOV	[EBP-020H],EAX		; Save instruction name length
	JMP	COMPARE_NAME
       ADDITION_TABLE:
	MOV	AL,0FH
	OR	[EBP-052H],AL		; Set addition opcode byte
	INC	ESI
	MOV	[EBP-040H],ESI		; Save pointer to next TBL86 item
       NEXT_INSTRUCTION:
	MOV	ESI,[EBP-040H]		; Move to next TBL86 structure
	XOR	EAX,EAX
	MOV	AL,[ESI]
	ADD	ESI,EAX
	MOV	[EBP-040H],ESI
	JMP	TEST_INSTRUCTION
       COMPARE_NAME:
	MOV	AL,[EDI]
	INC	EDI
	MOV	AH,AL			; To upper
	XLATB
	AND	AL,1
	SHL	AL,5
	NOT	AL
	AND	AH,AL
	LODSB
	CMP	AH,AL			; Compare characters
	JNE	NEXT_INSTRUCTION
	LOOP	COMPARE_NAME
	MOV	ESI,[EBP-040H]		; Load TBL86 structure
	MOV	AL,[ESI+01H]		; Opcode value
	MOV	[EBP-051H],AL		; Save opcode value
	MOV	AL,[ESI+02H]		; Load opcode mask
	CMP	AL,0F0H 		; Check instruction cc suffix
	JE	TEST_CC_SUFFIX
	MOV	EAX,[EBP-020H]		; Load instruction name length
	CMP	EAX,EDX 		; Test strings length
	JE	INSTRUCTION_SCANNED
	JMP	NEXT_INSTRUCTION
       TEST_CC_SUFFIX:
	MOV	ECX,EDX 		; Identifier length
	MOV	EAX,[EBP-020H]		; Load instruction name length
	SUB	ECX,EAX
	JBE	NEXT_INSTRUCTION	; Nothing to scan
	CMP	ECX,3			; CC suffix less than 3 character
	JG	NEXT_INSTRUCTION
	MOV	ESI,EDI 		; Get condition code value
	CALL	SCAN_CONDITION_CODE
	JC	NEXT_INSTRUCTION
	OR	[EBP-051H],AL		; Save cc field in opcode
       INSTRUCTION_SCANNED:
	MOV	ESI,[EBP-040H]		; Load TBL86 structure
	MOV	EAX,[ESI+04H]		; Get TBL86 flags
	AND	AL,088H 		; Clear ADDR and DATA flags
	PUSH	EAX
	MOV	EAX,[EBP-044H]		; Get DIS86 flags
	AND	EAX,000400F7H		; Leave only DATA/ADDR/SIB flags
	OR	[ESP],EAX		; Mix flags
	POP	EAX
	MOV	[EBP-044H],EAX		; Set instruction flags
	MOV	AL,[ESI+03H]		; Get group
	AND	AL,07H
	SHL	AL,3
	OR	[EBP-050H],AL		; Set group in modrm byte
	POP	ESI
	ADD	ESI,EDX 		; Eat instruction name
	POP	EDI
	POP	EDX
	POP	ECX
	CLC
	RET
       NOT_INSTRUCTION:
	POP	ESI
	POP	EDI
	POP	EDX
	POP	ECX
	STC
	RET


ASSEMBLE_INSTRUCTION:
	PUSH	EBX
	PUSH	EDX
	PUSH	ESI
	PUSH	ECX
	MOV	ESI,[EBP-040H]		; Load TBL86 structure
	LEA	ESI,[ESI+08H]		; Pointer to original operands
	LEA	EDX,[EBP-03CH]		; Pointer to operands array
	MOV	AX,[ESI]
	MOV	BX,[EDX]
	MOV	CX,[EDX+2]
	CALL	COMPARE_OPERANDS
	JC	CHECK_EXCHANGE
	MOV	AX,[ESI+2]
	MOV	BX,[EDX+2]
	MOV	CX,[EDX]
	CALL	COMPARE_OPERANDS
	JC	CHECK_EXCHANGE
	MOV	AX,[ESI+4]
	MOV	BX,[EDX+4]
	CALL	COMPARE_OPERANDS
	JC	ASSEMBLE_FAILED
	JMP	TEST_OPERANDS
       CHECK_EXCHANGE:
	MOV	EAX,[EBP-044H]		; Load instruction flags
	TEST	EAX,0200H		; Test bit D
	JZ	ASSEMBLE_FAILED
	MOV	AX,[ESI]
	MOV	BX,[EDX+2]
	MOV	CX,[EDX]
	CALL	COMPARE_OPERANDS
	JC	ASSEMBLE_FAILED
	MOV	AX,[ESI+2]
	MOV	BX,[EDX]
	MOV	CX,[EDX+2]
	CALL	COMPARE_OPERANDS
	JC	ASSEMBLE_FAILED
	MOV	AX,[ESI+4]
	MOV	BX,[EDX+4]
	CALL	COMPARE_OPERANDS
	JC	ASSEMBLE_FAILED
	MOV	AL,02H
	OR	[EBP-051H],AL		; Set bit D in opcode
	MOV	AX,[EDX]		; Exchange operands op1 <> op2
	MOV	BX,[EDX+2]
	MOV	[EDX],BX
	MOV	[EDX+2],AX
       TEST_OPERANDS:
	MOV	EBX,[EBP-044H]		; Load instruction flags
	TEST	EBX,0100H		; Set no bit W
	JZ	TEST_SIZEQ
	MOV	AL,1
	OR	[EBP-051H],AL
       TEST_SIZEQ:
	TEST	EBX,08000H		; Operands size equalense flag
	JZ	ASSIGN_OPERANDS
	MOV	AX,[EDX]
	MOV	AL,AH
	AND	AX,0F00FH		; AH type, AL size
	MOV	CX,[EDX+2]
	MOV	CL,CH
	AND	CX,0F00FH		; CH type, CL size
	CMP	AL,CL
	JE	ASSIGN_OPERANDS
	CMP	AH,070H 		; Test register with imm|mem
	JBE	TEST_REG_EQ
	XCHG	EAX,ECX
	JZ	ASSIGN_OPERANDS
	CMP	AH,070H
	JBE	TEST_REG_EQ
	CMP	AH,080H 		; Memory with imm
	JE	MEM_WITH_IMM
	XCHG	EAX,ECX
	CMP	AH,080H
	JE	MEM_WITH_IMM
	JMP	SIZE_NOT_MATCH
      MEM_WITH_IMM:
	OR	AL,AL			; Memory size not typed
	JZ	SIZE_NOT_MATCH
	JMP	REG_WITH_IMM		; No difference
      TEST_REG_EQ:
	CMP	CH,080H
	JE	REG_WITH_MEM
	CMP	CH,0E0H
	JE	REG_WITH_IMM
	JMP	ASSEMBLE_FAILED
      REG_WITH_IMM:
	CMP	AL,CL
	JB	SIZE_NOT_MATCH
	MOV	AH,[EDX+1]
	AND	AH,0F0H
	OR	AH,AL
	MOV	[EDX+1],AH
	MOV	AH,[EDX+3]
	AND	AH,0F0H
	OR	AH,AL
	MOV	[EDX+3],AH
	JMP	ASSIGN_OPERANDS
      REG_WITH_MEM:
	OR	CL,CL			; Memory size same as register size
	JZ	SET_MEM_EQ
	CMP	AL,CL			; Sizes mismatch
	JNE	SIZE_NOT_MATCH
       SET_MEM_EQ:
	MOV	AH,[EDX+1]
	AND	AH,0F0H
	OR	AH,AL
	MOV	[EDX+1],AH
	MOV	AH,[EDX+3]
	AND	AH,0F0H
	OR	AH,AL
	MOV	[EDX+3],AH
       ASSIGN_OPERANDS:
	XOR	ECX,ECX
	MOV	[EBP-020H],ECX
	MOV	ESI,[EBP-040H]		; Pointer to TBL86
	LEA	ESI,[ESI+08H]		; Load operands array
       ASSIGN_OP_LOOP:
	MOV	AX,[EDX]
	OR	AX,AX
	JZ	ASSIGN_NEXT
	MOV	CH,AH
	AND	CH,0F0H
	CMP	CH,0E0H
	JE	ASSIGN_IMM
	CMP	CH,070H
	JBE	ASSIGN_REG
	CMP	CH,080H
	JE	ASSIGN_MEM
	CMP	CH,0D0H 		; Far memory have no operands
	JE	ASSIGN_MEM
	JMP	NEXT_OP
       ASSIGN_IMM:
	TEST	EBX,08000H		; Test third operand size equlence
	JZ	CONST_1
	MOV	ECX,[EBP-020H]		; Test if IMM third operand
	CMP	CL,2
	JE	IMM_THIRD
       CONST_1:
	MOV	CX,[ESI]		; Pseudo constant 1
	CMP	CX,0E101H
	JNE	ALIGN_IMM
	MOV	ECX,[EBP-048H]
	CMP	ECX,1			; Constant not 1
	JNE	ASSEMBLE_FAILED
	JMP	NEXT_OP
       ALIGN_IMM:
	MOV	AL,[ESI+1]
	AND	AL,0FH			; Align on specified size
	OR	AL,AL
	JZ	SET_IMM_FLAG
	MOV	AH,AL
       SET_IMM_FLAG:
	MOVZX	EAX,AH
	SHL	EAX,4
	OR	[EBP-044H],EAX		; Set data flags
	JMP	NEXT_OP
       IMM_THIRD:
	MOV	CL,[ESI+1]		; Hard specified size
	AND	CL,0FH
	OR	CL,CL
	JZ	TEST_IMMEQ
	CMP	AH,CL			; Sizes mismatch
	JG	ASSEMBLE_FAILED
	MOV	AH,[ESI+1]		; Align on specified size
	AND	AH,0FH
	JMP	SET_IMM_FLAG
       TEST_IMMEQ:
	MOV	AL,[EBP-039H]		; Get previous operand size
	AND	AL,0FH
	CMP	AH,AL			; Need less size than previous
	JG	SIZE_NOT_MATCH
	MOV	CL,0E0H 		; Set sizes equalence
	OR	CL,AL
	MOV	[EDX+1H],CL
	MOV	AH,AL
	JMP	SET_IMM_FLAG
       ASSIGN_REG:
	MOV	AH,[ESI]
	MOV	AL,[EDX]
	AND	AL,0FH			; Get register index
	MOV	CH,AH
	AND	CH,0F0H 		; Location field
	CMP	CH,010H
	JE	REG_IN_OPCODE
	CMP	CH,020H
	JE	REG_IN_MODRG
	CMP	CH,040H
	JE	REG_IN_MODRM
	CMP	CH,050H
	JE	SEG_IN_OPCODE
	JMP	NEXT_OP
       REG_IN_OPCODE:
	OR	[EBP-051H],AL
	JMP	NEXT_OP
       REG_IN_MODRG:
	SHL	AL,3
	OR	[EBP-050H],AL
	JMP	NEXT_OP
       REG_IN_MODRM:
	OR	AL,0C0H 		; Set MOD field to 0C0H
	OR	[EBP-050H],AL
	JMP	NEXT_OP
       SEG_IN_OPCODE:
	SHL	AL,3
	OR	[EBP-051H],AL
	JMP	NEXT_OP
       ASSIGN_MEM:
	MOV	CH,[ESI+1]		; Test Direct offset
	AND	CH,0F0H
	CMP	CH,0B0H
	JE	ASSIGN_OFFS
	MOV	AH,4
	TEST	AL,010H 		; Address mode16 flag
	SETNZ	CL
	SHR	AH,CL			; Mode 2 or 4 (16bit or 32bit)
	MOV	ECX,[EBP-018H]		; Calc default address mode
	SHR	ECX,3
	CMP	AH,CL
	JE	NEXT_OP
	MOV	AL,067H
	MOV	[EBP-056H],AL		; Set address override prefix
	JMP	NEXT_OP
       ASSIGN_OFFS:
	MOV	EBX,[EBP-044H]		; Load DIS86 flags
	MOV	AL,BL
	AND	BL,0F0H 		; Clear address flags
	AND	AL,00FH 		; Get address size
	MOV	ECX,[EBP-018H]		; Calc current bits mode
	SHR	ECX,3
	CMP	AL,CL			; Too long offset for 16bit mode
	JG	ASSEMBLE_FAILED
	OR	BL,CL
	MOV	[EBP-044H],EBX
	JMP	NEXT_OP
       NEXT_OP:
	MOV	AL,[ESI+1]
	MOV	AH,AL
	AND	AH,0F0H 		; No override for far pointer
	CMP	AH,0D0H
	JE	START_ASSEMBLE
	AND	AL,00FH
	OR	AL,AL
	JNZ	TEST_BITW
	MOV	AL,[EDX+1]		; Test data size override
	AND	AL,0FH
	CMP	AL,2
	JE	TEST_PREFIX66
	CMP	AL,4
	JE	TEST_PREFIX66
	TEST	EBX,0100H
	JZ	ASSEMBLE_FAILED
	JMP	TEST_BITW
       TEST_PREFIX66:
	MOV	ECX,[EBP-018H]		; Calc default size
	SHR	ECX,3
	CMP	AL,CL
	JE	ASSIGN_NEXT
	MOV	AL,066H
	MOV	[EBP-057H],AL		; Set data override prefix
	JMP	ASSIGN_NEXT
       TEST_BITW:
	TEST	EBX,0100H		; Test byte size bit W
	JZ	ASSIGN_NEXT
	MOV	AL,[ESI+1]
	AND	AL,0FH
	OR	AL,AL
	JNZ	ASSIGN_NEXT
	MOV	AL,[EDX+1]
	AND	AL,0FH
	CMP	AL,1
	JNE	ASSIGN_NEXT
	NOT	AL
	AND	[EBP-051H],AL		; Clear bit W
       ASSIGN_NEXT:
	MOV	EAX,[EBP-020H]
	INC	EAX
	MOV	[EBP-020H],EAX
	ADD	EDX,2
	ADD	ESI,2
	CMP	AL,3
	JB	ASSIGN_OP_LOOP
       START_ASSEMBLE:
	MOV	EBX,[EBP-044H]		; Update flags
	LEA	EAX,[EDI+010H]		; Test maximal instruction length
	CMP	EAX,[EBP+0CH]
	JAE	OUT_OF_MEMORY
	MOV	ESI,[EBP-040H]		; Load instruction mode from TBL86
	MOV	AL,[ESI+03H]
	SHR	AL,4
	OR	AL,AL			; Normal instructon mode
	JZ	USE_DEF_MODE
	MOV	ECX,[EBP-018H]		; Get current bits mode
	SHR	ECX,3
	CMP	AL,CL			; No need data override prefix
	JE	USE_DEF_MODE
	MOV	AL,[ESI+01H]
	CMP	AL,0E3H 		; Fucking jcxz/jecxz use 67h prefix
	JNE	ADD_66PREFIX		;  to swithc bits mode!!!.
	MOV	AL,067H
	MOV	[EBP-056H],AL		; Set address override prefix
	JMP	USE_DEF_MODE
       ADD_66PREFIX:
	MOV	AL,066H
	MOV	[EBP-057H],AL		; Set data override prefix
       USE_DEF_MODE:
	MOV	[EBP-020H],EDI		; Save pointer to code start
	MOV	AL,[EBP-053H]		; Lock prefix
	OR	AL,AL
	JZ	ASM_REP
	STOSB
       ASM_REP:
	MOV	AL,[EBP-055H]		; Repeat prefix
	OR	AL,AL
	JZ	ASM_SEG
	STOSB
       ASM_SEG:
	MOV	AL,[EBP-054H]		; Segment override prefix
	OR	AL,AL
	JZ	ASM_P67
	STOSB
       ASM_P67:
	MOV	AL,[EBP-056H]		; Address override prefix
	OR	AL,AL
	JZ	ASM_P66
	STOSB
       ASM_P66:
	MOV	AL,[EBP-057H]		; Data override prefix
	OR	AL,AL
	JZ	ASM_OPCODE
	STOSB
       ASM_OPCODE:
	MOV	AL,[EBP-052H]		; Opcode addition byte 0Fh
	OR	AL,AL
	JZ	ASM_OPCODE2
	STOSB
       ASM_OPCODE2:
	MOV	AL,[EBP-051H]		; Opcode byte
	STOSB
	TEST	EBX,010000H		; Check MODRM
	JNZ	ASM_MODRM
	JMP	ASM_ADDRESS
       ASM_MODRM:
	MOV	AL,[EBP-050H]		; MODRM byte
	STOSB
	TEST	EBX,040000H		; Check SIB
	JZ	ASM_ADDRESS
	MOV	AL,[EBP-04FH]		; SIB byte
	STOSB
       ASM_ADDRESS:
	XOR	ECX,ECX 		; Get address size
	MOV	CL,BL
	AND	CL,07H
	OR	CL,CL
	JZ	ASM_IMM
	LEA	ESI,[EBP-04EH]		; Copy address
	REP	MOVSB
	MOV	AL,[EBP-03BH]		; Far address operand
	AND	AL,0F0H
	CMP	AL,0C0H
	JNE	ASM_IMM
	MOV	CL,2			; Copy selector
	LEA	ESI,[EBP-04AH]
	REP	MOVSB
       ASM_IMM:
	XOR	ECX,ECX 		; Get imm size
	MOV	CL,BL
	SHR	CL,4
	AND	CL,07H
	LEA	ESI,[EBP-048H]		; Copy imm
	REP	MOVSB
	MOV	EAX,EDI
	SUB	EAX,[EBP-020H]		; Calc instruction size
	MOV	ECX,[EBP-010H]		; Pass counter
	CMP	ECX,1			; No output at first pass
	JG	ASSEMBLE_OK
	MOV	EDI,[EBP-020H]		; Reset assemble buffer
       ASSEMBLE_OK:
	MOV	[EBP-04H],EAX		; Save assembled length
	POP	ECX
	POP	ESI
	POP	EDX
	POP	EBX
	CLC
	RET
       ASSEMBLE_FAILED:
	POP	ECX
	POP	ESI
	POP	EDX
	POP	EBX
	STC
	RET

ASSEMBLE_LINE:
	PUSH	EDX
	PUSH	ECX
	XOR	ECX,ECX
	MOV	[EBP-04H],ECX
	CALL	EAT_WHITESPACE
	OR	AH,AH			; End of file
	JZ	EOF_CHARACTER
	CMP	AH,0AH			; Line feed
	JE	LF_CHARACTER
	PUSH	ECX			; Clear DIS86 structure
	PUSH	EDI
	XOR	EAX,EAX
	MOV	ECX,30H
	LEA	EDI,[EBP-060H]		; Load pointer to start of DIS86
	REP	STOSB
	POP	EDI
	POP	ECX
	CALL	LOAD_TABLE_PTR		; Load instruction table
	ADD	EAX,8			; Skip table header
	MOV	[EBP-028H],ESI		; Save instruction name ptr
	CALL	SCAN_INSTRUCTION
	JC	ILLEGAL_INSTRUCTION
       OPERAND_LOOP:
	CALL	SCAN_OPERAND
	JC	INSTRUCTION_LOADED
	INC	ECX
	CMP	CL,3			; More than 3 operands
	JG	INVALID_OPERAND
	CALL	EAT_WHITESPACE
	CMP	AH,','			; Scan next operand
	JNE	INSTRUCTION_LOADED
	INC	ESI
	JMP	OPERAND_LOOP
       INSTRUCTION_LOADED:
	PUSH	ESI			; Save source ptr
       ASSEMBLE_NEXT:
	CALL	ASSEMBLE_INSTRUCTION	; Assemble
	JNC	LINE_ASSEMBLED
	MOV	EAX,[EBP-040H]		; Load TBL86 structure
	XOR	EDX,EDX 		; Try next encoding
	MOV	DL,[EAX]
	ADD	EAX,EDX
	MOV	ESI,[EBP-028H]		; Load instruction name ptr
	CALL	SCAN_INSTRUCTION	; No available instructions
	JC	INVALID_OPERAND
	JMP	ASSEMBLE_NEXT
       LINE_ASSEMBLED:
	POP	ESI			; Restore source ptr
	CALL	EAT_WHITESPACE
	OR	AH,AH			; End of file
	JZ	EOF_CHARACTER
	CMP	AH,0AH			; Line feed
	JE	LF_CHARACTER
	JMP	EXTRA_CHARACTERS
       LF_CHARACTER:
	INC	ESI
	MOV	EAX,[EBP-04H]		; Instruction length
	POP	ECX
	POP	EDX
	CLC
	RET
       EOF_CHARACTER:
	MOV	EAX,[EBP-04H]		; Instruction length
	POP	ECX
	POP	EDX
	STC
	RET


ASSEMBLE:
	ENTER	060H,0
	PUSHA
	MOV	[EBP-08H],ESP		; Save ESP
	CALL	LOAD_CHARTYPE_TABLE	; Pointer to character table
	MOV	EBX,EAX
	MOV	ESI,[EBP+010H]		; Load source code pointer
	MOV	EDI,[EBP+08H]		; Load memory buffer
	MOV	EDX,EDI
	MOV	ECX,024H		; Allocate ASM86 header
	LEA	EAX,[EDI+ECX]
	CMP	EAX,[EBP+0CH]
	JAE	OUT_OF_MEMORY
	XOR	EAX,EAX 		; Clear ASM86 structure
	REP	STOSB
	MOV	EAX,1			; First pass
	MOV	[EBP-010H],EAX		; local pass counter
	MOV	[EDX+020H],EAX		; ASM86 pass counter
	MOV	EAX,[EBP+014H]		; Set instruction pointer
	MOV	[EBP-014H],EAX
	MOV	ECX,1
	MOV	[EDX+01CH],ECX		; Set ASM86 lines counter
	DEC	ECX			; Clear code size counter
	MOV	EAX,[EBP+018H]		; Get default bits
	MOV	[EBP-018H],EAX		; Set bits mode
       FIRST_PASS:
	CALL	ASSEMBLE_LINE
	JC	SECOND_PASS
	ADD	ECX,EAX 		; Increase code size counter
	ADD	[EBP-014H],EAX		; Increase instruction pointer
	MOV	EAX,[EDX+01CH]		; Increase ASM86 lines counter
	INC	EAX
	MOV	[EDX+01CH],EAX
	JMP	FIRST_PASS
       SECOND_PASS:
	ADD	ECX,EAX 		; Add result of last assembling
	MOV	[EDX+04H],ECX		; Size of code (not optimized)
	LEA	EAX,[EDI+ECX]		; Calc wanted memory size
	MOV	ECX,EAX
	SUB	ECX,[EBP+08H]
	MOV	[EDX+018H],ECX		; Wanted size
	CMP	EAX,[EBP+0CH]		; Check memory limit
	JAE	LOW_MEMORY
	MOV	[EDX+08H],EDI		; Set pointer to code output
	MOV	EAX,[EBP-010H]		; Increase current pass
	INC	EAX
	MOV	[EBP-010H],EAX		; Local pass counter
	MOV	[EDX+020H],EAX		; ASM86 pass counter
	MOV	ESI,[EBP+010H]		; Restore source code pointer
	MOV	EAX,[EBP+014H]		; Set instruction pointer
	MOV	[EBP-014H],EAX
	MOV	EAX,[EBP+018H]		; Get default bits
	MOV	[EBP-018H],EAX		; Set bits mode
	XOR	ECX,ECX 		; Clear code size counter
       PRODUCE_CODE:
	CALL	ASSEMBLE_LINE
	JC	SOURCE_ASSEMBLED
	ADD	ECX,EAX 		; Increase code size counter
	ADD	[EBP-014H],EAX		; Increase instruction pointer
	JMP	PRODUCE_CODE
       SOURCE_ASSEMBLED:
	ADD	ECX,EAX 		; Add result of last assembling
	MOV	[EDX+04H],ECX		; Size of code
	MOV	EAX,EDI 		; Calc used memory
	SUB	EAX,[EBP+08H]
	MOV	[EDX+014H],EAX		; Used memory
	XOR	EAX,EAX
	MOV	[EBP-04H],EAX		; Return no errors
	JMP	ADE86_RETURN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; DISASSEMBLER                                                            ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

UNASSEMBLE:
	ENTER	0CH,0
	PUSHA
	MOV	EAX,[EBP+0CH]		; Correct pointer to end of code
	INC	EAX
	MOV	[EBP+0CH],EAX
	XOR	EBX,EBX 		; Instruction flags
	MOV	[EBP-08H],ESP
	MOV	ESI,[EBP+08H]		; Pointer to code
	MOV	EDI,[EBP+010H]		; Pointer to DIS86
	XOR	EAX,EAX 		; Clear DIS86 structure
	MOV	ECX,030H
	REP	STOSB
	MOV	EDI,[EBP+010H]		; Restore pointer to code
	MOV	EAX,[EBP+018H]		; Get current bits
	CMP	EAX,010H
	JNE	DECODE_PREFIX
	OR	BX,07000H		; Setup 16bits mode
       DECODE_PREFIX:
	CMP	ESI,[EBP+0CH]		; Check
	JAE	CROSS_MEMORY
	MOV	AL,[ESI]
	CMP	AL,0F2H
	JE	STORE_REP_PREFIX
	CMP	AL,0F3H
	JE	STORE_REP_PREFIX
	CMP	AL,0F0H
	JE	STORE_LOCK_PREFIX
	CMP	AL,026H
	JE	STORE_SEG_PREFIX
	CMP	AL,02EH
	JE	STORE_SEG_PREFIX
	CMP	AL,036H
	JE	STORE_SEG_PREFIX
	CMP	AL,03EH
	JE	STORE_SEG_PREFIX
	CMP	AL,064H
	JE	STORE_SEG_PREFIX
	CMP	AL,065h
	JE	STORE_SEG_PREFIX
	CMP	AL,067h
	JE	STORE_ADDR_PREFIX
	CMP	AL,066h
	JE	STORE_DATA_PREFIX
	JMP	DECODE_INSTRUCTION
       STORE_REP_PREFIX:
	MOV	[EDI+0BH],AL		; Repeat prefix
	INC	ESI
	JMP	DECODE_PREFIX
       STORE_SEG_PREFIX:
	MOV	[EDI+0CH],AL		; Segment override prefix
	INC	ESI
	JMP	DECODE_PREFIX
       STORE_LOCK_PREFIX:
	MOV	[EDI+0DH],AL		; Bus lock prefix
	INC	ESI
	JMP	DECODE_PREFIX
       STORE_DATA_PREFIX:
	CMP	[EDI+09H],AL		; Data override prefix
	JE	DECODE_PREFIX
	INC	ESI
	XOR	BX,02000H		; Switch data mode
	JMP	DECODE_PREFIX
       STORE_ADDR_PREFIX:
	CMP	[EDI+0AH],AL
	JE	DECODE_PREFIX
	INC	ESI
	XOR	BX,01000H		; Switch address mode
	JMP	DECODE_PREFIX
      DECODE_INSTRUCTION:
	XOR	ECX,ECX
	CALL	LOAD_TABLE_PTR
	MOV	EDX,EAX
	ADD	EDX,8			; Pointer to main table
	LODSB
	CMP	AL,0FH
	JNE	SEARCH_OPCODE
	CALL	LOAD_TABLE_PTR
	ADD	EAX,[EAX]		; Seek to addition table
	MOV	EDX,EAX
	MOV	[EDI+0EH],AL		; Set 0F opcode
	OR	BX,0400H		; Set 2-byte opcode flag
	CMP	ESI,[EBP+0CH]
	JG	CROSS_MEMORY
	LODSB
      SEARCH_OPCODE:
	MOV	AH,[EDX]
	CMP	AH,0EEH 		; Addition table signature
	JE	UNKNOWN_INSTRUCTION
	CMP	AH,0FFH 		; End of table signature
	JE	UNKNOWN_INSTRUCTION
	MOV	AH,AL
	AND	AH,[EDX+02H]		; AND with search mask
	CMP	AH,[EDX+01H]		; Compare with base opcode
	JE	CHECK_INSTRUCTION
	JMP	NEXT_OPCODE
       NEXT_OPCODE:
	MOV	CL,[EDX]		; Move to next instruction
	MOVZX	ECX,CL
	ADD	EDX,ECX
	JMP	SEARCH_OPCODE
       CHECK_INSTRUCTION:
	MOV	AH,[EDX+01H]		; Check opcode
	CMP	AH,0E3H
	JNE	JCXZ_DECODED
	TEST	EBX,01000H
	JZ	JCXZ_DECODED
	OR	EBX,02000H
       JCXZ_DECODED:
	MOV	AH,[EDX+03H]		; Load instruction mode
	AND	AH,0F0H
	OR	AH,AH
	JZ	CHECK_GROUP_INDEX
	TEST	EBX,02000H
	JNZ	CHECK_MODE16
       TEST_MODE16:
	CMP	AH,040H
	JNE	NEXT_OPCODE
	JMP	CHECK_GROUP_INDEX
       CHECK_MODE16:
	CMP	AH,020H
	JNE	NEXT_OPCODE
       CHECK_GROUP_INDEX:
	MOV	ECX,[EDX+04H]		; Load TBL86 flags
	TEST	ECX,020000H
	JZ	OPCODE_FOUNDED
	MOV	AH,[EDX+03H]		; Load group index
	AND	AH,0FH
	MOV	CL,[ESI]
	SHR	CL,3
	AND	CL,7
	CMP	CL,AH
	JE	OPCODE_FOUNDED
	JMP	NEXT_OPCODE
       OPCODE_FOUNDED:
	MOV	[EDI+020H],EDX		; Save TBL86 pointer
	MOV	[EDI+0FH],AL		; Save opcode
	OR	EBX,[EDX+04H]		; OR with TBL86 flags
	MOV	AH,[EDX+02H]		; Load search mask
       CHECK_BIT_W:
	CMP	AH,0FEH
	JE	DECODE_BIT_W
	CMP	AH,0FDH
	JE	DECODE_BIT_W
	JMP	DECODE_MODRM
       DECODE_BIT_W:
	TEST	AL,1
	JNZ	DECODE_MODRM
	TEST	BL,040H 		; Check DATAX flag
	JZ	DECODE_MODRM
	AND	BL,08FH 		; Force 1 byte data
	OR	BL,010H
       DECODE_MODRM:
	TEST	EBX,010000H		; Check MODRM flag
	JZ	DECODE_ADDRESS
	CMP	ESI,[EBP+0CH]
	JAE	CROSS_MEMORY
	LODSB
	MOV	[EDI+010H],AL		; Save MODRM byte
	MOV	AH,AL			; MOD into AL, RM into AH
	AND	AH,7
	SHR	AL,6
	CMP	AL,3			; No memory expression part
	JE	DECODE_DISP
	TEST	BX,01000H		; Test address mode
	JZ	DECODE_MODRM32
	CMP	AL,0
	JE	MODRM16_MOD0
	;SHL    AL,4
	OR	BL,AL			; Set 8/16bit displacement
	JMP	DECODE_DISP
       MODRM16_MOD0:
	CMP	AH,6
	JNE	DECODE_DISP
	OR	BL,02H			; Set 16bit displacement
	JMP	DECODE_DISP
       DECODE_MODRM32:
	CMP	AH,4
	JNE	MODRM32_DISP
       DECODE_SIB:
	OR	EBX,040000H		; Set SIB flag
	CMP	ESI,[EBP+0CH]
	JAE	CROSS_MEMORY
	LODSB
	MOV	[EDI+011H],AL		; Save SIB byte
	MOV	AH,AL
	AND	AH,7
       MODRM32_DISP:
	MOV	AL,[EDI+010H]		; Load MOD field
	SHR	AL,6
	CMP	AL,0
	JE	MODRM32_MOD0
	CMP	AL,1
	JE	MODRM32_MOD1
	CMP	AL,2
	JE	MODRM32_MOD2
	JMP	INVALID_ADDRESS
       MODRM32_MOD0:
	CMP	AH,5
	JNE	DECODE_DISP
	OR	BL,04H			; Set 32bit displacement
	JMP	DECODE_DISP
       MODRM32_MOD1:
	OR	BL,01H			; Set 8bit displacement
	JMP	DECODE_DISP
       MODRM32_MOD2:
	OR	BL,04H			; Set 32bit displacement
	JMP	DECODE_DISP
       DECODE_DISP:
	XOR	EAX,EAX
	MOV	AL,BL			; Get address size
	AND	AL,7
	CMP	AL,4
	JNE	DISP_SIZE_OK
	MOV	EAX,[EBP+018H]		; Calc address size
	SHR	EAX,3
	TEST	BX,01000H		; 16bit address
	JZ	DISP_SIZE_OK
	MOV	AL,2
       DISP_SIZE_OK:
	CMP	AL,1
	JE	DISP1
	CMP	AL,2
	JE	DISP2
	CMP	AL,4
	JE	DISP4
	JMP	DECODE_IMM
       DISP1:
	LODSB
	MOVZX	EAX,AL
	MOV	[EDI+012H],EAX
	JMP	DECODE_IMM
       DISP2:
	LODSW
	MOVZX	EAX,AX
	MOV	[EDI+012H],EAX
	JMP	DECODE_IMM
       DISP4:
	LODSD
	MOV	[EDI+012H],EAX
	JMP	DECODE_IMM
       DECODE_ADDRESS:
	XOR	EAX,EAX
	MOV	AL,BL			; Get address size
	AND	AL,7
	CMP	AL,6
	JE	DECODE_FAR
	CMP	AL,4
	JNE	ADDR_SIZE_OK
	MOV	EAX,[EBP+018H]		; Calc address size
	SHR	EAX,3
	TEST	BX,02000H		; 16bit address
	JZ	ADDR_SIZE_OK
	MOV	AL,2
       ADDR_SIZE_OK:
	LEA	ECX,[ESI+EAX]
	CMP	ECX,[EBP+0CH]
	JAE	CROSS_MEMORY
	CMP	AL,1
	JE	ADDRESS1
	CMP	AL,2
	JE	ADDRESS2
	CMP	AL,4
	JE	ADDRESS4
	JMP	DECODE_IMM
       ADDRESS1:
	MOV	AL,[ESI]
	TEST	AL,AL
	SETS	CL
	MOVSX	EAX,AL
	MOV	[EBP-010H],EAX
	LODSB
	MOVZX	EAX,AL
	JMP	CHECK_RELATIVE
       ADDRESS2:
	MOV	AX,[ESI]
	TEST	AX,AX
	SETS	CL
	MOVSX	EAX,AX
	MOV	[EBP-010H],EAX
	LODSW
	MOVZX	EAX,AX
	JMP	CHECK_RELATIVE
       ADDRESS4:
	LODSD
	TEST	EAX,EAX
	SETS	CL
	MOV	[EBP-010H],EAX
       CHECK_RELATIVE:
	MOV	[EDI+012H],EAX		; Save address constant
	TEST	BL,08H			; Relative address flag
	JZ	DECODE_IMM
	CMP	CL,1
	JE	BACK_RELATIVE
	MOV	ECX,ESI
	SUB	ECX,[EBP+08H]
	ADD	EAX,[EBP+014H]		; +current origin
	ADD	EAX,ECX 		; +instruction length
	MOV	[EDI+012H],EAX
	JMP	DECODE_IMM
       BACK_RELATIVE:
	MOV	EAX,[EBP-010H]
	NEG	EAX			; abs(address)
	MOV	ECX,ESI
	SUB	ECX,[EBP+08H]
	SUB	EAX,ECX 		; address-instruction length
	MOV	ECX,[EBP+014H]
	SUB	ECX,EAX 		; origin-address constant
	MOV	[EDI+012H],ECX
	JMP	DECODE_IMM
       DECODE_FAR:
	TEST	BX,02000H		; Chec data mode
	JZ	FAR_ADDRESS32
	LEA	EAX,[ESI+04H]
	CMP	EAX,[EBP+0CH]
	JAE	CROSS_MEMORY
	LODSW
	MOVZX	ECX,AX
	LODSW
	MOV	[EDI+012H],ECX		; Save address
	MOV	[EDI+016H],AX		; Save selector
	JMP	DECODE_IMM
       FAR_ADDRESS32:
	LEA	EAX,[ESI+06H]
	CMP	EAX,[EBP+0CH]
	JAE	CROSS_MEMORY
	LODSD
	MOV	ECX,EAX
	LODSW
	MOV	[EDI+012H],ECX		; Save address
	MOV	[EDI+016H],AX		; Save selector
	JMP	DECODE_IMM
       DECODE_IMM:
	XOR	EAX,EAX
	MOV	AL,BL			; Get data size
	SHR	AL,4
	AND	AL,7
	CMP	AL,4
	JNE	IMM_SIZE_OK
	MOV	EAX,[EBP+018H]
	SHR	EAX,3
	TEST	BX,02000H		; 16bit mode
	JZ	IMM_SIZE_OK
	MOV	AL,2
       IMM_SIZE_OK:
	LEA	ECX,[ESI+EAX]
	CMP	ECX,[EBP+0CH]
	JAE	CROSS_MEMORY
	CMP	AL,1
	JE	IMMDATA1
	CMP	AL,2
	JE	IMMDATA2
	CMP	AL,3
	JE	IMMDATA3
	CMP	AL,4
	JE	IMMDATA4
	JMP	UNASSEMBLE_OK
       IMMDATA1:
	LODSB
	MOV	[EDI+018H],AL
	JMP	DECODE_OP
       IMMDATA2:
	LODSW
	MOV	[EDI+018H],AX
	JMP	DECODE_OP
       IMMDATA3:
	LODSW
	MOV	[EDI+018H],AX
	LODSB
	MOV	[EDI+01AH],AL
	JMP	DECODE_OP
       IMMDATA4:
	LODSD
	MOV	[EDI+018H],EAX
	JMP	DECODE_OP
       DECODE_OP:
       UNASSEMBLE_OK:
	MOV	[EDI+01CH],EBX		; Save flags
	MOV	EAX,ESI
	SUB	EAX,[EBP+08H]
	CMP	EAX,0FH
	JG	SUPERFLUOUS_PREFIX
	MOV	[EDI+08H],AL		; Save instruction length
	MOV	EAX,[EBP+018H]
	MOV	[EDI+04H],EAX		; Save IP
	XOR	EAX,EAX
	JMP	ADE86_RETURN


REASSEMBLE:
	RET

PRINT_REGISTER:
	PUSH	ECX
	PUSH	ESI
	CALL	DELTA11
       DELTA11:
	POP	ESI
	ADD	ESI,GENERAL_REGISTER-DELTA11
	CMP	AH,011H
	JE	PRINT_R8
	CMP	AH,012H
	JE	PRINT_R16
	CMP	AH,014H
	JE	PRINT_R32
	CMP	AH,072H
	JE	PRINT_S16
	CMP	AH,024H
	JE	PRINT_CR32
	CMP	AH,034H
	JE	PRINT_DR32
	CMP	AH,0F4H
	JE	PRINT_TR32
       PRINT_R8:
	MOVZX	ECX,AL
	SHL	ECX,1
	LEA	ESI,[ESI+ECX]
	MOVSB
	MOVSB
	JMP	REGISTER_PRINTED
       PRINT_R16:
	MOVZX	ECX,AL
	SHL	ECX,1
	LEA	ESI,[ESI+ECX+010H]
	MOVSB
	MOVSB
	JMP	REGISTER_PRINTED
       PRINT_R32:
	MOV	CL,'E'
	MOV	[EDI],CL
	INC	EDI
	JMP	PRINT_R16
       PRINT_S16:
	MOVZX	ECX,AL
	SHL	ECX,1
	LEA	ESI,[ESI+ECX+020H]
	MOVSB
	MOVSB
	JMP	REGISTER_PRINTED
       PRINT_CR32:
	MOV	AH,AL
	MOV	AL,'C'
	STOSB
	MOV	AL,'R'
	STOSB
	MOV	AL,AH
	ADD	AL,30H
	STOSB
	JMP	REGISTER_PRINTED
       PRINT_DR32:
	MOV	AH,AL
	MOV	AL,'D'
	STOSB
	MOV	AL,'R'
	STOSB
	MOV	AL,AH
	ADD	AL,30H
	STOSB
	JMP	REGISTER_PRINTED
       PRINT_TR32:
	MOV	AH,AL
	MOV	AL,'T'
	STOSB
	MOV	AL,'R'
	STOSB
	MOV	AL,AH
	ADD	AL,30H
	STOSB
       REGISTER_PRINTED:
	POP	ESI
	POP	ECX
	RET

PRINT_CONST:
	PUSH	ECX
	MOV	AL,'+'
	MOV	[EDI],AL
	MOV	EAX,[ESP+0CH]		; Argument 2 constant size
	MOV	ECX,[ESP+010H]		; Argument 3 constant value
	CMP	AL,1
	JE	PRINT_C1
	CMP	AL,2
	JE	PRINT_C2
	CMP	AL,4
	JE	PRINT_C4
       PRINT_C1:
	ROL	ECX,28
	TEST	CL,08H
	JNZ	SIGN_MINUS
	JMP	CONST_TESTED
       PRINT_C2:
	ROL	ECX,20
	TEST	CL,08H
	JNZ	SIGN_MINUS
	JMP	CONST_TESTED
       PRINT_C4:
	ROL	ECX,4
	TEST	CL,08H
	JNZ	SIGN_MINUS
	JMP	CONST_TESTED
       SIGN_MINUS:
	MOV	EAX,[ESP+08H]		; Argument 1 need sign character if 1
	OR	EAX,EAX
	JZ	CONST_TESTED
	ROR	ECX,4
	NEG	ECX
	ROL	ECX,4
	MOV	AL,'-'
	MOV	[EDI],AL
       CONST_TESTED:
	MOV	EAX,[ESP+08H]		; Argument 1 need sign character if 1
	OR	EAX,EAX
	SETNZ	AL			; Eat sign character
	ADD	EDI,EAX
	MOV	EAX,[ESP+0CH]		; Argument 2 constant size
	MOV	AH,AL
	SHL	AH,1			; Calc characters count
	MOV	AL,CL
	AND	AL,0FH
	CMP	AL,9
	JBE	PRINT_DIGIT
	MOV	AL,'0'
	STOSB
       PRINT_DIGIT:
	MOV	AL,CL			; Print hex character (4bit value)
	AND	AL,0FH
	CMP	AL,9
	JNG	NEXT_DIGIT
	ADD	AL,7
       NEXT_DIGIT:
	ADD	AL,30H
	STOSB
	ROL	ECX,4
	DEC	AH
	CMP	AH,0
	JNZ	PRINT_DIGIT
	MOV	AL,'H'
	STOSB
	POP	ECX
	RET

PRINT_RM16_REGS:
	PUSH	ESI
	PUSH	ECX
	XOR	ECX,ECX
	CALL	DELTA12
       DELTA12:
	POP	ESI
	ADD	ESI,RM16_ADDRESSING-DELTA12
       RM16_PRINT_LOOP:
	LODSB
	CMP	AL,0
	JZ	RM16_PRINTED
	MOV	CL,AL
	LODSB
	CMP	AH,AL
	JE	PRINT_RM16_ITEM
	ADD	ESI,ECX
	JMP	RM16_PRINT_LOOP
       PRINT_RM16_ITEM:
	REP	MOVSB
       RM16_PRINTED:
	POP	ECX
	POP	ESI
	RET


PRINT_DIS86:
	ENTER	0CH,0
	PUSHA
	MOV	EAX,3
	MOV	[EBP-0CH],EAX
	MOV	[EBP-08H],ESP		; Save ESP value
	MOV	EDX,[EBP+010H]		; Load DIS86 pointer
	MOV	ESI,[EDX+020H]		; Load instruction name from TBL86
	MOV	EDI,[EBP+08H]		; Load output buffer
	LEA	ESI,[ESI+0EH]
	XOR	ECX,ECX
       PRINT_INAME:
	LODSB
	STOSB
	INC	ECX
	OR	AL,AL
	JNZ	PRINT_INAME
	DEC	EDI
	MOV	EAX,10H 		; Align command operand position
	SUB	EAX,ECX
	MOV	ECX,EAX
	MOV	AL,20H			; Fill with spaces
	STOSB
	XOR	ECX,ECX
	;REP    STOSB
	MOV	EBX,[EDX+01CH]		; DIS86 flags
	MOV	ESI,[EDX+020H]		; Load TBL86 operands
	PUSH	EDI
	LEA	ESI,[ESI+08H]
	LEA	EDI,[EDX+024H]
	MOV	ECX,3
	REP	MOVSW			; Copy operands
	LEA	ESI,[EDX+024H]
	POP	EDI
	TEST	BX,0200H		; Test bit D
	JZ	PRINT_OP
	MOV	AL,[EDX+0FH]
	TEST	AL,2
	JZ	PRINT_OP
	MOV	AX,[ESI]		; Exchange operands
	MOV	CX,[ESI+2]
	MOV	[ESI],CX
	MOV	[ESI+2],AX
       PRINT_OP:
	LODSW
	OR	AX,AX
	JZ	DIS86_PRINTED
	MOV	CH,AH
	AND	CH,0FH			; Size mask
	CMP	AH,0A0H
	JE	CHK_ADDR_SIZE
	CMP	AH,0B0H
	JE	CHK_ADDR_SIZE
	CMP	AH,010H
	JE	CHK_VRMI_SIZE
	CMP	AH,080H
	JE	CHK_VRMI_SIZE
	CMP	AH,0E0H
	JE	CHK_VRMI_SIZE
	CMP	AH,090H
	JE	CHK_VRMI_SIZE
	JMP	OP_SIZE_OK
       CHK_ADDR_SIZE:
	MOV	CH,2			; Get default size
	TEST	BX,04000H
	SETZ	CL
	SHL	CH,CL
	TEST	BX,02000H		; 16bit mode
	JZ	OP_SIZE_OK
	MOV	CH,2
	JMP	OP_SIZE_OK
       CHK_VRMI_SIZE:
	MOV	CH,2			; Get default size
	TEST	BX,02000H
	SETZ	CL
	SHL	CH,CL
	TEST	BX,02000H		; 16bit mode
	JZ	CHK_BYTE_SIZE
	MOV	CH,2
       CHK_BYTE_SIZE:
	TEST	BX,0100H		; Test bit W
	JZ	OP_SIZE_OK
	MOV	CL,[EDX+0FH]
	TEST	CL,1
	JNZ	OP_SIZE_OK
	MOV	CH,1
       OP_SIZE_OK:
	OR	AH,CH
	MOV	CL,AH
	AND	CL,0F0H 		; Type mask
	CMP	CL,010H
	JE	PRINT_REG
	CMP	CL,020H
	JE	PRINT_CRX
	CMP	CL,030H
	JE	PRINT_DRX
	CMP	CL,040H
	JE	PRINT_STX
	CMP	CL,050H
	JE	PRINT_MMX
	CMP	CL,070H
	JE	PRINT_SEG
	CMP	CL,080H
	JE	PRINT_MEM
	CMP	CL,090H
	JE	PRINT_RM
	CMP	CL,0A0H
	JE	PRINT_REL
	CMP	CL,0B0H
	JE	PRINT_OFFS
	CMP	CL,0C0H
	JE	PRINT_FAR
	CMP	CL,0D0H
	JE	PRINT_FARPTR
	CMP	CL,0E0H
	JE	PRINT_IMM
	CMP	CL,0F0H
	JE	PRINT_TRX
	JMP	PRINT_NEXT
       GET_REG_INDEX:
	MOV	CL,AL
	AND	CL,0F0H
	CMP	CL,10H
	JE	REG_000XXX
	CMP	CL,20H
	JE	MOD_XXX000
	CMP	CL,40H
	JE	MOD_000XXX
	AND	AL,7
	CALL	PRINT_REGISTER
	JMP	PRINT_NEXT
       REG_000XXX:
	MOV	CL,[EDX+0FH]
	AND	CL,7
	MOV	AL,CL
	CALL	PRINT_REGISTER
	JMP	PRINT_NEXT
       MOD_000XXX:
	MOV	CL,[EDX+010H]
	AND	CL,7
	MOV	AL,CL
	CALL	PRINT_REGISTER
	JMP	PRINT_NEXT
       MOD_XXX000:
	MOV	CL,[EDX+010H]
	SHR	CL,3
	AND	CL,7
	MOV	AL,CL
	CALL	PRINT_REGISTER
	JMP	PRINT_NEXT
       PRINT_REG:
	JMP	GET_REG_INDEX
       PRINT_CRX:
	JMP	GET_REG_INDEX
       PRINT_DRX:
	JMP	GET_REG_INDEX
       PRINT_STX:
	JMP	GET_REG_INDEX
       PRINT_MMX:
	JMP	GET_REG_INDEX
       PRINT_SEG:
	JMP	GET_REG_INDEX
       PRINT_TRX:
	JMP	GET_REG_INDEX
       PRINT_RM:
	MOV	CL,[EDX+010H]
	AND	CL,0C0H
	CMP	CL,0C0H
	JE	RM_AS_REG
       RM_AS_MEM:
	AND	AH,08FH
	JMP	PRINT_MEM
       RM_AS_REG:
	AND	AH,01FH
	JMP	PRINT_REG
       PRINT_REL:
	MOV	ECX,[EDX+012H]
	PUSH	ECX
	PUSH	4
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_NEXT
       PRINT_OFFS:
	MOV	AL,'['
	STOSB
	MOV	ECX,[EDX+012H]		; Address constant
	PUSH	ECX
	AND	AH,0FH			; Address size
	MOVZX	ECX,AH
	PUSH	ECX
	PUSH	0
	CALL	PRINT_CONST
	MOV	AL,']'
	STOSB
	JMP	PRINT_NEXT
       PRINT_FAR:
	MOV	ECX,[EDX+016H]		; Selector constant
	PUSH	ECX
	MOV	CH,AH			; Save address size
	PUSH	2
	PUSH	0
	CALL	PRINT_CONST
	MOV	AL,':'
	STOSB
	XOR	EAX,EAX
	MOV	AL,CH
	MOV	ECX,[EDX+012H]		; Address constant
	PUSH	ECX
	PUSH	EAX			; Address size
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_NEXT
       PRINT_IMM:
	TEST	AL,01H			; Pseudo constant 1
	JNZ	PRINT_01
	MOV	ECX,[EDX+018H]		; Imm constant
	PUSH	ECX
	AND	AH,0FH			; Constant size
	MOVZX	ECX,AH
	PUSH	ECX
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_NEXT
       PRINT_01:
	PUSH	1			; Psuedo constant value=1
	PUSH	1
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_NEXT
       PRINT_FARPTR:
	MOV	AL,'F'
	STOSB
	MOV	AL,'A'
	STOSB
	MOV	AL,'R'
	STOSB
	MOV	AL,' '
	STOSB
       PRINT_MEM:
	MOV	AL,'['
	STOSB
	MOV	CL,[EDX+010H]		; Get MODRM byte
	MOV	CH,CL
	AND	CH,0C0H 		; MOD filed mask
	AND	CL,7			; RM field mask
	TEST	BX,01000H		; Test RM mode
	JZ	PRINT_RM32
	CMP	CX,06			; MOD==0 RM==6
	JZ	RM16_DISP_ONLY
	MOV	AH,CL
	CALL	PRINT_RM16_REGS
	OR	CH,CH			; Nothing in displacement
	JZ	PRINT_MEM_OK
	MOVZX	EAX,CH			; Address size
	SHR	EAX,6
	MOV	ECX,[EDX+012H]
	PUSH	ECX			; Displacement value
	PUSH	EAX
	PUSH	1			; Need sign
	CALL	PRINT_CONST
	JMP	PRINT_MEM_OK
       RM16_DISP_ONLY:
	MOV	ECX,[EDX+012H]
	PUSH	ECX			; Displacement value
	PUSH	2
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_MEM_OK
       PRINT_RM32:
	CMP	CL,4			; Print SIB byte
	JE	PRINT_SIB
	CMP	CX,05			; MOD==0 RM==5 Displacement only
	JE	RM32_DISP_ONLY
	MOV	AL,CL
	MOV	AH,14H
	CALL	PRINT_REGISTER
	JMP	PRINT_DISP32
       PRINT_SIB:
	MOV	CL,[EDX+011H]
	AND	CL,7
	CMP	CX,05H
	JE	PRINT_INDEX
	MOV	AL,CL
	MOV	AH,14H
	CALL	PRINT_REGISTER
	MOV	CL,[EDX+011H]
	SHR	CL,3
	AND	CL,7
	CMP	CL,4
	JE	PRINT_SIB_DISP
	MOV	AL,'+'
	STOSB
	JMP	PRINT_INDEX
       PRINT_INDEX:
	MOV	AL,[EDX+011H]
	SHR	AL,3
	AND	AL,7
	CMP	AL,4
	JE	PRINT_SIB_DISP
	MOV	AH,014H
	CALL	PRINT_REGISTER
	MOV	CL,[EDX+011H]
	SHR	CL,6
	OR	CL,CL
	JZ	PRINT_SIB_DISP
	MOV	AL,'*'
	STOSB
	CMP	CL,1
	JE	PRINT_S2
	CMP	CL,2
	JE	PRINT_S4
       PRINT_S8:
	MOV	AL,'8'
	STOSB
	JMP	PRINT_SIB_DISP
       PRINT_S4:
	MOV	AL,'4'
	STOSB
	JMP	PRINT_SIB_DISP
       PRINT_S2:
	MOV	AL,'2'
	STOSB
       PRINT_SIB_DISP:
	MOV	CL,[EDX+011H]
	CMP	CX,05H
	JE	RM32_DISP_ONLY
	AND	CL,7
	CMP	CX,05H
	JNE	PRINT_DISP32
	MOV	ECX,[EDX+012H]
	OR	ECX,ECX
	JZ	PRINT_MEM_OK
	PUSH	ECX
	PUSH	4
	PUSH	1
	CALL	PRINT_CONST
	JMP	PRINT_MEM_OK
       RM32_DISP_ONLY:
	MOV	ECX,[EDX+012H]
	PUSH	ECX			; Displacement value
	PUSH	4
	PUSH	0
	CALL	PRINT_CONST
	JMP	PRINT_MEM_OK
       PRINT_DISP32:
	OR	CH,CH
	JZ	PRINT_MEM_OK
	CMP	CH,040H
	SETE	CL
	MOV	CH,4
	SHR	CH,CL
	SHR	CH,CL
	MOV	EAX,[EDX+012H]
	OR	EAX,EAX
	JZ	PRINT_MEM_OK
	PUSH	EAX
	MOVZX	EAX,CH
	PUSH	EAX
	PUSH	1
	CALL	PRINT_CONST
	JMP	PRINT_MEM_OK
       PRINT_MEM_OK:
	MOV	AL,']'
	STOSB
	JMP	PRINT_NEXT
       PRINT_NEXT:
	MOV	AX,[ESI]
	OR	AX,AX
	JZ	VOID_OPERAND
	MOV	AL,','
	STOSB
       VOID_OPERAND:
	MOV	ECX,[EBP-0CH]
	DEC	ECX
	JZ	DIS86_PRINTED
	MOV	[EBP-0CH],ECX
	JMP	PRINT_OP
       DIS86_PRINTED:
	JMP	ADE86_RETURN
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; ERRORS                                                                  ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ADE86_RETURN:
	MOV	ESP,[EBP-08H]		; Restore ESP
	POPA				; Restore all registers
	MOV	EAX,[EBP-04H]		; Load return value
	LEAVE
	RET

ASM86_ERROR:
	MOV	ESI,[EBP+08H]		; Load ASM86/DIS86 structure
	MOV	[ESI],EAX		; Set error code
	MOV	[EBP-04H],EAX		; Return error
	JMP	ADE86_RETURN

UAS86_ERROR:
	MOV	[EDI],EAX
	MOV	[EBP-04H],EAX
	JMP	ADE86_RETURN

OUT_OF_MEMORY:
	MOV	EAX,1
	JMP	ASM86_ERROR

LOW_MEMORY:
	MOV	EAX,2
	JMP	ASM86_ERROR

UNKNOWN_INSTRUCTION:
	MOV	EAX,3
	JMP	UAS86_ERROR

ILLEGAL_INSTRUCTION:
	MOV	EAX,4
	JMP	ASM86_ERROR

INVALID_OPERAND:
	MOV	EAX,5
	JMP	ASM86_ERROR

EXTRA_CHARACTERS:
	MOV	EAX,6
	JMP	ASM86_ERROR

INVALID_ADDRESS:
	MOV	EAX,7
	JMP	ASM86_ERROR

OUT_OF_RANGE:
	MOV	EAX,8
	JMP	ASM86_ERROR

CROSS_MEMORY:
	MOV	EAX,9
	JMP	UAS86_ERROR

SIZE_NOT_MATCH:
	MOV	EAX,0Ah
	JMP	ASM86_ERROR

SUPERFLUOUS_PREFIX:
	MOV	EAX,0BH
	JMP	UAS86_ERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; COMMON STRUCTURES                                                       ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GENERAL_REGISTER:
	DB 'AL'
	DB 'CL'
	DB 'DL'
	DB 'BL'
	DB 'AH'
	DB 'CH'
	DB 'DH'
	DB 'BH'
GENERAL_REGISTER16:
	DB 'AX'
	DB 'CX'
	DB 'DX'
	DB 'BX'
	DB 'SP'
	DB 'BP'
	DB 'SI'
	DB 'DI'
SEGMENT_REGISTER:
	DB 'ES'
	DB 'CS'
	DB 'SS'
	DB 'DS'
	DB 'FS'
	DB 'GS'
CONDITION_CODE:
	DB 1,00H,'O'
	DB 2,01H,'NO'
	DB 1,02H,'B'
	DB 1,02H,'C'
	DB 2,03H,'NB'
	DB 2,03H,'NC'
	DB 1,04H,'E'
	DB 1,04H,'Z'
	DB 2,05H,'NZ'
	DB 2,05H,'NE'
	DB 2,06H,'BE'
	DB 2,06H,'NA'
	DB 1,07H,'A'
	DB 3,07H,'NBE'
	DB 1,08H,'S'
	DB 2,09H,'NS'
	DB 1,0AH,'P'
	DB 2,0AH,'PE'
	DB 2,0BH,'PO'
	DB 2,0BH,'NP'
	DB 1,0CH,'L'
	DB 3,0CH,'NGE'
	DB 2,0DH,'GE'
	DB 2,0DH,'NL'
	DB 2,0EH,'LE'
	DB 2,0EH,'NG'
	DB 1,0FH,'G'
	DB 3,0FH,'NLE'
	DB 0
SIZE_QUALIFIER:
	DB 4,01H,'BYTE'
	DB 4,02H,'WORD'
	DB 5,04H,'DWORD'
	DB 5,06H,'FWORD'
	DB 5,08H,'QWORD'
	DB 0
PTRTYPE_QUALIFIER:
	DB 5,01H,'SHORT'
	DB 4,02H,'NEAR'
	DB 3,04H,'FAR'
	DB 0
RM16_ADDRESSING:
	DB 5,00H,'BX+SI'
	DB 5,01H,'BX+DI'
	DB 5,02H,'BP+SI'
	DB 5,03H,'BP+DI'
	DB 2,04H,'SI'
	DB 2,05H,'DI'
	DB 2,06H,'BP'
	DB 2,07H,'BX'
	DB 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; CHARACTER TABLE                                                         ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LOAD_CHARTYPE_TABLE:
	CALL	DELTA1
       DELTA1:
	POP	EAX
	ADD	EAX,CHARACTER_TABLE-DELTA1
	RET
	ALIGN 2

CHARACTER_TABLE:

;    00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
;00:
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,08H,00H,00H,00H,08H,00H,00H
;10:
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;20:
 DB  08H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;30: '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
 DB  06H,06H,06H,06H,06H,06H,06H,06H,06H,06H,00H,00H,00H,00H,00H,00H
;40:     'A' 'B' 'C' 'D' 'E' 'F'
 DB  00H,05H,05H,05H,05H,05H,05H,01H,01H,01H,01H,01H,01H,01H,01H,01H
;50:                                         'Z'
 DB  01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,00H,00H,00H,00H,00H
;60:     'a' 'b' 'c' 'd' 'e' 'f'
 DB  00H,05H,05H,05H,05H,05H,05H,01H,01H,01H,01H,01H,01H,01H,01H,01H
;70:                                         'z'
 DB  01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,00H,00H,00H,00H,00H
;80
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;90
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;A0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;B0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;C0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;D0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;E0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
;F0
 DB  00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                         ;;
;; INSTRUCTION TABLE                                                       ;;
;;                                                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LOAD_TABLE_PTR:
	CALL	DELTA2
       DELTA2:
	POP	EAX
	ADD	EAX,INSTRUCTION_TABLE-DELTA2
	RET

INSTRUCTION_TABLE:
	; Offset to second table
	DD INSTRUCTION_TABLE2-INSTRUCTION_TABLE
	; Size of table at all
	DD END_OF_TABLE-INSTRUCTION_TABLE

	; ADD Ev Gv
	DB 012H,000H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'ADD',0

	; ADD EAX Iv
	DB 012H,004H,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'ADD',0

	; PUSH ES
	DB 014H,006H,0FFH,00H
	DD 0
	DW 07208H,0,0
	DB 'PUSH',0,0

	; POP ES
	DB 012H,007H,0FFH,00H
	DD 0
	DW 07208H,0,0
	DB 'POP',0

	; OR Ev Gv
	DB 012H,008H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'OR',0,0

	; OR EAX Iv
	DB 012H,00CH,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'OR',0,0

	; PUSH CS
	DB 014H,00EH,0FFH,00H
	DD 0
	DW 07209H,0,0
	DB 'PUSH',0,0

	; ADC Ev Gv
	DB 012H,010H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'ADC',0

	; ADC EAX Iv
	DB 012H,014H,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'ADC',0

	; PUSH SS
	DB 014H,016H,0FFH,00H
	DD 0
	DW 0720AH,0,0
	DB 'PUSH',0,0

	; POP SS
	DB 012H,017H,0FFH,00H
	DD 0
	DW 0720AH,0,0
	DB 'POP',0

	; SBB Ev Gv
	DB 012H,018H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'SBB',0

	; SBB EAX Iv
	DB 012H,01CH,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'SBB',0

	; PUSH DS
	DB 014H,01EH,0FFH,00H
	DD 0
	DW 0720BH,0,0
	DB 'PUSH',0,0

	; POP DS
	DB 012H,01FH,0FFH,00H
	DD 0
	DW 0720BH,0,0
	DB 'POP',0

	; AND Ev Gv
	DB 012H,020H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'AND',0

	; AND EAX Iv
	DB 012H,024H,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'AND',0

	; DAA
	DB 012H,027H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'DAA',0

	; SUB Ev Gv
	DB 012H,028H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'SUB',0

	; SUB EAX Iv
	DB 012H,02CH,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'SUB',0

	; DAS
	DB 012H,02FH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'DAS',0

	; XOR Ev Gv
	DB 012H,030H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'XOR',0

	; XOR EAX Iv
	DB 012H,034H,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'XOR',0

	; AAA
	DB 012H,037H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'AAA',0

	; CMP Ev Gv
	DB 012H,038H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITW+BITD
	DW 09040H, 01020H, 0
	DB 'CMP',0

	; CMP EAX Iv
	DB 012H,03CH,0FEH,00H
	DD 00008140H			;SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'CMP',0

	; AAS
	DB 012H,03FH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'AAS',0

	; INC Gv
	DB 012H,040H,0F8H,00H
	DD 0
	DW 01010H, 0, 0
	DB 'INC',0

	; DEC Gv
	DB 012H,048H,0F8H,00H
	DD 0
	DW 01010H, 0, 0
	DB 'DEC',0

	; PUSH Gv
	DB 014H,050H,0F8H,00H
	DD 0
	DW 01010H, 0, 0
	DB 'PUSH',0,0

	; POP Gv
	DB 012H,058H,0F8H,00H
	DD 0
	DW 01010H, 0, 0
	DB 'POP',0

	; PUSHAW
	DB 016H,060H,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'PUSHAW',0,0

	; PUSHAD
	DB 016H,060H,0FFH,40H		; 32bit mode
	DD 0
	DW 0, 0, 0
	DB 'PUSHAD',0,0

	; PUSHA
	DB 014H,060H,0FFH,00H
	DD 0
	DW 0, 0, 0
	DB 'PUSHA',0

	; POPAW
	DB 014H,061H,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'POPAW',0

	; POPAD
	DB 014H,061H,0FFH,40H		; 32bit mode
	DD 0
	DW 0, 0, 0
	DB 'POPAD',0

	; POPA
	DB 014H,061H,0FFH,00H
	DD 0
	DW 0, 0, 0
	DB 'POPA',0,0

	; BOUND Gv Mv
	DB 014H,062H,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H, 08000H, 0
	DB 'BOUND',0

	; ARPL Ew Gw
	DB 014H,063H,0FFH,00H
	DD 00010000H			; MODRM
	DW 09240H, 01220H, 00000H
	DB 'ARPL',0,0

	; PUSH Iv
	DB 014H,068H,0FFH,00H
	DD 00000040H			; DATAX
	DW 0E008H, 0, 0
	DB 'PUSH',0,0

	; IMUL Gv Ev Ib
	DB 014H,06BH,0FFH,00H
	DD 00018010H			; MODRM+SIZEQ+DATAX
	DW 01020H, 09040H, 0E100H
	DB 'IMUL',0,0

	; IMUL Gv Ev Iv
	DB 014H,069H,0FFH,00H
	DD 00018040H			; MODRM+SIZEQ+DATAX
	DW 01020H, 09040H, 0E000H
	DB 'IMUL',0,0

	; PUSH Ib
	DB 014H,06AH,0FFH,00H
	DD 00000010H			; DATA1
	DW 0E108H, 0, 0
	DB 'PUSH',0,0

	; INSB
	DB 014H,06CH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'INSB',0,0

	; INSW
	DB 014H,06DH,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'INSW',0,0

	; INSD
	DB 014H,06DH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'INSD',0,0

	; OUTSB
	DB 014H,06EH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'OUTSB',0

	; OUTSW
	DB 014H,06FH,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'OUTSW',0

	; OUTSD
	DB 014H,06FH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'OUTSD',0

	; JCC SHORT
	DB 010H,070H,0F0H,00H
	DD 00100809H			; JMP+CC+ADDR1+REL
	DW 0A100H, 00000H, 00000H
	DB 'J',0


	; GROUP2 Ev Ib                  ; MODRM+MODEX+BITW+DATAX
	DB 012H,082H,0FEH,00H		; Group index 0
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'ADD',0
	;
	DB 012H,082H,0FEH,01H		; Group index 1
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'OR',0,0
	;
	DB 012H,082H,0FEH,02H		; Group index 2
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'ADC',0
	;
	DB 012H,082H,0FEH,03H		; Group index 3
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'SBB',0
	;
	DB 012H,082H,0FEH,04H		; Group index 4
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'AND',0
	;
	DB 012H,082H,0FEH,05H		; Group index 5
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'SUB',0
	;
	DB 012H,082H,0FEH,06H		; Group index 6
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'XOR',0
	;
	DB 012H,082H,0FEH,07H		; Group index 7
	DD 00030140H
	DW 09040H, 0E108H, 0
	DB 'CMP',0
	;END OF GROUP2


	; GROUP1 Ev Iv                  ; MODRM+MODEX+SIZEQ+BITW+DATAX
	DB 012H,080H,0FEH,00H		; Group index 0
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'ADD',0
	;
	DB 012H,080H,0FEH,01H		; Group index 1
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'OR',0,0
	;
	DB 012H,080H,0FEH,02H		; Group index 2
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'ADC',0
	;
	DB 012H,080H,0FEH,03H		; Group index 3
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'SBB',0
	;
	DB 012H,080H,0FEH,04H		; Group index 4
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'AND',0
	;
	DB 012H,080H,0FEH,05H		; Group index 5
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'SUB',0
	;
	DB 012H,080H,0FEH,06H		; Group index 6
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'XOR',0
	;
	DB 012H,080H,0FEH,07H		; Group index 7
	DD 00038140H
	DW 09040H, 0E000H, 0
	DB 'CMP',0
	;END OF GROUP1

	; TEST eAX Iv
	DB 014H,0A8H,0FEH,00H
	DD 00008140H			; SIZEQ+BITW+DATAX
	DW 01008H, 0E000H, 0
	DB 'TEST',0,0

	; TEST Ev Gv
	DB 014H,084H,0FFH,00H
	DD 00018100H			; MODRM+SIZEQ+BITW
	DW 09040H, 01020H, 0
	DB 'TEST',0,0

	; NOP
	DB 012H,090H,0FFH,00H
	DD 0
	DW 0, 0, 0
	DB 'NOP',0

	; XCHG eAX Gv
	DB 014H,090H,0F8H,00H
	DD 00008000H			; SIZEQ
	DW 01008H, 01010H, 0
	DB 'XCHG',0,0

	; XCHG Ev Gv
	DB 014H,084H,0FFH,00H
	DD 00018100H			; MODRM+SIZEQ+BITW
	DW 09040H, 01020H, 0
	DB 'XCHG',0,0

	; MOV eAX Ov
	DB 012H,0A0H,0FCH,00H
	DD 00008304H			; SIZEQ+BITW+BITD+ADDRX
	DW 01008H, 0B000H, 0
	DB 'MOV',0

	; MOV Ev Gv
	DB 012H,088H,0FCH,00H
	DD 00018300H			; MODRM+SIZEQ+BITD+BITW
	DW 09040H, 01020H, 0
	DB 'MOV',0

	; MOV Ew Sw
	DB 012H,08CH,0FDH,00H
	DD 00018200H			; MODRM+SIZEQ+BITD
	DW 09240H, 07220H, 0
	DB 'MOV',0

	; LEA Gv,Mv
	DB 012H,08DH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H, 08000H, 0
	DB 'LEA',0

	; POP Ev
	DB 012H,08FH,0FFH,00H		; Group index 0
	DD 00030000H			; MODRM+MODEX
	DW 09040H, 0, 0
	DB 'POP',0

	; CBW
	DB 012H,098H,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'CBW',0

	; CWDE
	DB 014H,098H,0FFH,40H		; 32bit mode
	DD 0
	DW 0, 0, 0
	DB 'CWDE',0,0

	; CWD
	DB 012H,099H,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'CWD',0

	; CDQ
	DB 012H,099H,0FFH,40H		; 32bit mode
	DD 0
	DW 0, 0, 0
	DB 'CDQ',0

	; CALL FAR
	DB 014H,09AH,0FFH,00H
	DD 00A0000EH			; CALL+FAR+ADDR2+ADDRX
	DW 0C000H, 0, 0
	DB 'CALL',0,0


	; PUSHF
	DB 014H,09CH,0FFH,00H
	DD 0
	DW 0, 0, 0
	DB 'PUSHF',0

	; PUSHFW
	DB 016H,09CH,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'PUSHFW',0,0

	; PUSHFD
	DB 016H,09CH,0FFH,20H		; 32bit mode
	DD 0
	DW 0, 0, 0
	DB 'PUSHFD',0,0

	; POPF
	DB 014H,09DH,0FFH,00H
	DD 0
	DW 0, 0, 0
	DB 'POPF',0,0

	; POPFW
	DB 014H,09DH,0FFH,20H		; 16bit mode
	DD 0
	DW 0, 0, 0
	DB 'POPFW',0

	; POPFD
	DB 014H,09DH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'POPFD',0

	; SAHF
	DB 014H,09EH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'SAHF',0,0

	; LAHF
	DB 014H,09FH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'LAHF',0,0

	; MOVSB
	DB 014H,0A4H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'MOVSB',0

	; MOVSW
	DB 014H,0A4H,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'MOVSW',0

	; MOVSD
	DB 014H,0A5H,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'MOVSD',0

	; CMPSB
	DB 014H,0A4H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CMPSB',0

	; CMPSW
	DB 014H,0A7H,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'CMPSW',0

	; CMPSD
	DB 014H,0A7H,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'CMPSD',0

	; STOSB
	DB 014H,0AAH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'STOSB',0

	; STOSW
	DB 014H,0ABH,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'STOSW',0

	; STOSD
	DB 014H,0ABH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'STOSD',0

	; LODSB
	DB 014H,0ACH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'LODSB',0

	; LODSW
	DB 014H,0ADH,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'LODSW',0

	; LODSD
	DB 014H,0ADH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'LODSD',0

	; SCASB
	DB 014H,0AEH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'SCASB',0

	; SCASW
	DB 014H,0AFH,0FFH,20H		; 16bit mode
	DD 0
	DW 0,0,0
	DB 'SCASW',0

	; SCASD
	DB 014H,0AFH,0FFH,40H		; 32bit mode
	DD 0
	DW 0,0,0
	DB 'SCASD',0

	; MOV Gb Ib
	DB 012H,0B0H,0F8H,00H
	DD 00008010H			; SIZEQ+DATA1
	DW 01110H, 0E100H, 0
	DB 'MOV',0

	; MOV Gv Iv
	DB 012H,0B8H,0F8H,00H
	DD 00008040H			; SIZEQ+DATAX
	DW 01010H, 0E000H, 0
	DB 'MOV',0


	; GROUP4 Ev 1                   ; MODRM+MODEX+BITW
	; ROL
	DB 012H,0D0H,0FEH,00H		; Group index 0
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'ROL',0
	; ROR
	DB 012H,0D0H,0FEH,01H		; Group index 1
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'ROR',0
	; RCL
	DB 012H,0D0H,0FEH,02H		; Group index 2
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'RCL',0
	; RCR
	DB 012H,0D0H,0FEH,03H		; Group index 3
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'RCR',0
	; SHL
	DB 012H,0D0H,0FEH,04H		; Group index 4
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'SHL',0
	; SHR
	DB 012H,0D0H,0FEH,05H		; Group index 5
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'SHR',0
	; SAR
	DB 012H,0D0H,0FEH,07H		; Group index 7
	DD 00030100H
	DW 09040H, 0E101H, 0
	DB 'SAR',0
	;END OF GROUP4


	; GROUP3 Ev Ib                  ; MODRM+MODEX+BITW+DATA1
	; ROL
	DB 012H,0C0H,0FEH,00H		; Group index 0
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'ROL',0
	; ROR
	DB 012H,0C0H,0FEH,01H		; Group index 1
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'ROR',0
	; RCL
	DB 012H,0C0H,0FEH,02H		; Group index 2
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'RCL',0
	; RCR
	DB 012H,0C0H,0FEH,03H		; Group index 3
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'RCR',0
	; SHL
	DB 012H,0C0H,0FEH,04H		; Group index 4
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'SHL',0
	; SHR
	DB 012H,0C0H,0FEH,05H		; Group index 5
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'SHR',0
	; SAR
	DB 012H,0C0H,0FEH,07H		; Group index 7
	DD 00030110H
	DW 09040H, 0E100H, 0
	DB 'SAR',0
	;END OF GROUP3

	; RET Iw
	DB 012H,0C2H,0FFH,00H
	DD 00000020H			; DATA2
	DW 0E200H,0,0
	DB 'RET',0

	; RETN
	DB 012H,0C3H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'RET',0

	; LES Gw Md
	DB 012H,0C4H,0FFH,20H		; 16bit mode
	DD 00010000H			; MODRM
	DW 01220H, 08400H, 0
	DB 'LES',0

	; LES Gd Mf
	DB 012H,0C4H,0FFH,40H		; 32bit mode
	DD 00010000H			; MODRM
	DW 01420H, 08600H, 0
	DB 'LES',0

	; LDS Gw Md
	DB 012H,0C5H,0FFH,20H		; 16bit mode
	DD 00010000H			; MODRM
	DW 01220H, 08400H, 0
	DB 'LDS',0

	; LDS Gd Mf
	DB 012H,0C5H,0FFH,40H		; 32bit mode
	DD 00010000H			; MODRM
	DW 01420H, 08600H, 0
	DB 'LDS',0

	; MOV Ev Iv
	DB 012H,0C6H,0FEH,00H		; Group index 0
	DD 00038140H			; MODRM+MODEX+SIZEQ+BITW+DATAX
	DW 09040H, 0E000H, 0
	DB 'MOV',0

	; ENTER
	DB 014H,0C8H,0FFH,00H
	DD 00000030H			; DATA1+DATA2
	DW 0E200H, 0E100H, 0
	DB 'ENTER',0

	; LEAVE
	DB 014H,0C9H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'LEAVE',0

	; RETF Iw
	DB 014H,0CAH,0FFH,00H
	DD 00000020H			; DATA2
	DW 0E200H,0,0
	DB 'RETF',0,0

	; RETF
	DB 014H,0CBH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'RETF',0,0

	; INT3
	DB 014H,0CCH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'INT3',0,0

	; INT Ib
	DB 012H,0CDH,0FFH,00H
	DD 00000010H			; DATA1
	DW 0E100H,0,0
	DB 'INT',0

	; INTO
	DB 014H,0CEH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'INTO',0,0

	; IRET
	DB 014H,0CFH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'IRET',0,0


	; GROUP5 Ev CL                  ; MODRM+MODEX+BITW
	; ROL
	DB 012H,0D2H,0FEH,00H		; Group index 0
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'ROL',0
	; ROR
	DB 012H,0D2H,0FEH,01H		; Group index 1
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'ROR',0
	; RCL
	DB 012H,0D2H,0FEH,02H		; Group index 2
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'RCL',0
	; RCR
	DB 012H,0D2H,0FEH,03H		; Group index 3
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'RCR',0
	; SHL
	DB 012H,0D2H,0FEH,04H		; Group index 4
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'SHL',0
	; SHR
	DB 012H,0D2H,0FEH,05H		; Group index 5
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'SHR',0
	; SAR
	DB 012H,0D2H,0FEH,07H		; Group index 7
	DD 00030100H
	DW 09040H, 01109H, 0
	DB 'SAR',0
	;END OF GROUP4

	; AAM Ib
	DB 012H,0D4H,0FFH,00H
	DD 00000010H			; DATA1
	DW 0E100H,0,0
	DB 'AAM',0

	; AAD Ib
	DB 012H,0D5H,0FFH,00H
	DD 00000010H			; DATA1
	DW 0E100H,0,0
	DB 'AAD',0

	; XLATB
	DB 014H,0D7H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'XLATB',0

	; LOOPNZ
	DB 016H,0E0H,0FFH,00H
	DD 00000009H			; ADDR1+REL
	DW 0A100H,0,0
	DB 'LOOPNZ',0,0

	; LOOPZ
	DB 014H,0E1H,0FFH,00H
	DD 00000009H			; ADDR1+REL
	DW 0A100H,0,0
	DB 'LOOPZ',0

	; LOOP
	DB 014H,0E2H,0FFH,00H
	DD 00000009H			; ADDR1+REL
	DW 0A100H,0,0
	DB 'LOOP',0,0

	; JCXZ
	DB 014H,0E3H,0FFH,20H		; 16bit mode
	DD 00000009H			; ADDR1+REL
	DW 0A100H,0,0
	DB 'JCXZ',0,0

	; JECXZ
	DB 014H,0E3H,0FFH,40H		; 32bit mode
	DD 00000009H			; ADDR1+REL
	DW 0A100H,0,0
	DB 'JECXZ',0

	; IN eAX Ib
	DB 012H,0A4H,0FEH,00H
	DD 00000110H			; BITW+DATA1
	DW 01008H, 0E100H, 0
	DB 'IN',0,0

	; OUT Ib eAX
	DB 012H,0A6H,0FEH,00H
	DD 00000110H			; BITW+DATA1
	DW 0E100H, 01008H, 0
	DB 'OUT',0

	; CALL NEAR
	DB 014H,0E8H,0FFH,00H
	DD 0020000CH			; CALL+ADDRX+REL
	DW 0A000H, 0, 0
	DB 'CALL',0,0

	; JMP NEAR
	DB 012H,0E9H,0FFH,00H
	DD 0010000CH			; JMP+ADDRX+REL
	DW 0A000H, 0, 0
	DB 'JMP',0

	; JMP FAR
	DB 012H,0EAH,0FFH,00H
	DD 0090000EH			; JMP+FAR+ADDR2+ADDRX
	DW 0C000H, 0, 0
	DB 'JMP',0

	; JMP SHORT
	DB 012H,0EBH,0FFH,00H
	DD 00100009H			; JMP+ADDRX+REL
	DW 0A100H, 0, 0
	DB 'JMP',0

	; IN eAX DX
	DB 012H,0ACH,0FEH,00H
	DD 00000100H			; BITW
	DW 01008H, 0120AH, 0
	DB 'IN',0,0

	; OUT DX eAX
	DB 012H,0AEH,0FEH,00H
	DD 00000100H			; BITW
	DW 0120AH, 01008H, 0
	DB 'OUT',0

	; HLT
	DB 012H,0F4H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'HLT',0

	; CMC
	DB 012H,0F5H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CMC',0

	; TEST Ev Iv
	DB 014H,0F6H,0FEH,00H		; Group index 0
	DD 00038140H			; MODRM+MODEX+SIZEQ+BITW+DATAX
	DW 09040H, 0E000H, 0
	DB 'TEST',0,0

	; GROUP Ev                      ; MODRM+MODEX+BITW
	; NOT
	DB 012H,0F6H,0FEH,02H		; Group index 2
	DD 00030100H
	DW 09040H,0,0
	DB 'NOT',0
	; NEG
	DB 012H,0F6H,0FEH,03H		; Group index 3
	DD 00030100H
	DW 09040H,0,0
	DB 'NEG',0
	; MUL
	DB 012H,0F6H,0FEH,04H		; Group index 4
	DD 00030100H
	DW 09040H,0,0
	DB 'MUL',0
	; IMUL
	DB 014H,0F6H,0FEH,05H		; Group index 5
	DD 00030100H
	DW 09040H,0,0
	DB 'IMUL',0,0
	; DIV
	DB 012H,0F6H,0FEH,06H		; Group index 6
	DD 00030100H
	DW 09040H,0,0
	DB 'DIV',0
	; IDIV
	DB 014H,0F6H,0FEH,07H		; Group index 7
	DD 00030100H
	DW 09040H,0,0
	DB 'IDIV',0,0
	;END OF GROUP

	; CLC
	DB 012H,0F8H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CLC',0

	; STC
	DB 012H,0F9H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'STC',0

	; CLI
	DB 012H,0FAH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CLI',0

	; STI
	DB 012H,0FBH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'STI',0

	; CLD
	DB 012H,0FCH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CLD',0

	; STD
	DB 012H,0FDH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'STD',0

	; INC Ev
	DB 012H,0FEH,0FEH,00H		; Group index 0
	DD 00030100H			; MODRM+MODEX+BITW
	DW 09040H,0,0
	DB 'INC',0

	; DEC Ev
	DB 012H,0FEH,0FEH,01H		; Group index 1
	DD 00030100H			; MODRM+MODEX+BITW
	DW 09040H,0,0
	DB 'DEC',0

	; CALL Ev
	DB 014H,0FFH,0FFH,02H		; Group index 2
	DD 00030000H			; MODRM+MODEX
	DW 09040H,0,0
	DB 'CALL',0,0

	; CALL Mv
	DB 014H,0FFH,0FFH,03H		; Group index 3
	DD 00830000H			; FAR+MODRM+MODEX
	DW 0D000H,0,0
	DB 'CALL',0,0

	; JMP Ev
	DB 012H,0FFH,0FFH,04H		; Group index 4
	DD 00030000H			; MODRM+MODEX
	DW 09040H,0,0
	DB 'JMP',0

	; JMP Mv
	DB 012H,0FFH,0FFH,05H		; Group index 5
	DD 00130000H			; FAR+MODRM+MODEX
	DW 0D000H,0,0
	DB 'JMP',0

	; PUSH Ev
	DB 014H,0FFH,0FFH,03H		; Group index 6
	DD 00030000H			; MODRM+MODEX
	DW 09040H,0,0
	DB 'PUSH',0,0

	; Addition table signature
	DB 0EEH,01H

INSTRUCTION_TABLE2:

	; SLDT Mw
	DB 014H,000H,0FFH,00H		; Group index 0
	DD 00030000H			; MODRM+MODEX
	DW 08240H,0,0
	DB 'SLDT',0,0

	; SLDT Gv
	DB 014H,000H,0FFH,00H		; Group index 0
	DD 00030000H			; MODRM+MODEX
	DW 01040H,0,0
	DB 'SLDT',0,0

	; STR Mw
	DB 012H,000H,0FFH,01H		; Group index 1
	DD 00030000H
	DW 08240H,0,0
	DB 'STR',0

	; STR Gv
	DB 012H,000H,0FFH,01H		; Group index 1
	DD 00030000H
	DW 01040H,0,0
	DB 'STR',0

	; LLDT Ew
	DB 014H,000H,0FFH,02H		; Group index 2
	DD 00030000H			; MODRM+MODEX
	DW 09240H,0,0
	DB 'LLDT',0,0

	; LTR Ew
	DB 012H,000H,0FFH,03H		; Group index 3
	DD 00030000H
	DW 09240H,0,0
	DB 'LTR',0

	; VERR Ew
	DB 014H,000H,0FFH,04H		; Group index 4
	DD 00030000H
	DW 09240H,0,0
	DB 'VERR',0,0

	; VERW Ew
	DB 014H,000H,0FFH,05H		; Group index 5
	DD 00030000H
	DW 09240H,0,0
	DB 'VERW',0,0

	; SGDT Ms
	DB 014H,001H,0FFH,00H		; Group index 0
	DD 00030000H
	DW 8640H,0,0
	DB 'SGDT',0,0

	; SIDT Ms
	DB 014H,001H,0FFH,01H		; Group index 1
	DD 00030000H
	DW 8640H,0,0
	DB 'SIDT',0,0

	; LGDT Ms
	DB 014H,001H,0FFH,02H		; Group index 2
	DD 00030000H
	DW 8640H,0,0
	DB 'LGDT',0,0

	; LIDT Ms
	DB 014H,001H,0FFH,03H		; Group index 3
	DD 00030000H
	DW 8640H,0,0
	DB 'LIDT',0,0

	; SMSW Ew
	DB 014H,001H,0FFH,04H		; Group index 4
	DD 00030000H
	DW 09240H,0,0
	DB 'SMSW',0,0

	; LMSW Ew
	DB 014H,001H,0FFH,06H		; Group index 6
	DD 00030000H
	DW 09240H,0,0
	DB 'LMSW',0,0

	; INVLPG Mb
	DB 016H,001H,0FFH,07H		; Group index 7
	DD 00030000H
	DW 08140H,0,0
	DB 'INVLPG',0,0

	; LAR Gv Ev
	DB 012H,002H,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H,09040H,0
	DB 'LAR',0

	; LSL Gv Ev
	DB 012H,003H,0FFH,00H
	DD 00018000H
	DW 01020H,09040H,0
	DB 'LSL',0

	; CLTS
	DB 014H,006H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CLTS',0,0

	; INVD
	DB 014H,008H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'INVD',0,0

	; WBINVD
	DB 016H,009H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'WBINVD',0,0

	; UD2
	DB 012H,00BH,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'UD2',0

	; MOV Gv CRX
	DB 012H,020H,0FDH,00H
	DD 00018200H			; MODRM+SIZEQ+BITD
	DW 01440H,02420H,0
	DB 'MOV',0

	; MOV Gv DRX
	DB 012H,021H,0FDH,00H
	DD 00018200H			; MODRM+SIZEQ+BITD
	DW 01440H,03420H,0
	DB 'MOV',0

	; MOV Gv TRX
	DB 012H,024H,0FDH,00H
	DD 00018200H			; MODRM+SIZEQ+BITD
	DW 01440H,0F420H,0
	DB 'MOV',0

	; WRMSR
	DB 014H,030H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'WRMSR',0

	; RDTSC
	DB 014H,031H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'RDTSC',0

	; RDMSR
	DB 014H,032H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'RDMSR',0

	; RDPMC
	DB 014H,033H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'RDPMC',0

	; SYSENTER
	DB 018H,034H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'SYSENTER',0,0

	; SYSEXIT
	DB 016H,035H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'SYSEXIT',0

	; CMOVcc Gv Ev
	DB 014H,040H,0F0H,00H
	DD 00018800H			; MODRM+SIZEQ+CC
	DW 01020H,09040H,0
	DB 'CMOV',0,0

	; Jcc NEAR
	DB 010H,080H,0F0H,00H
	DD 0010080CH			; JMP+CC+ADDRX+REL
	DW 0A000H,0,0
	DB 'J',0

	; SETcc Eb
	DB 012H,090H,0F0H,00H
	DD 00010800H			; MODRM+CC
	DW 09140H,0,0
	DB 'SET',0

	; PUSH FS
	DB 014H,0A0H,0FFH,00H
	DD 0
	DW 0720CH,0,0
	DB 'PUSH',0,0

	; POP FS
	DB 012H,0A1H,0FFH,00H
	DD 0
	DW 0720CH,0,0
	DB 'POP',0

	; CPUID
	DB 014H,0A2H,0FFH,00H
	DD 0
	DW 0,0,0
	DB 'CPUID',0

	; BT Ev Gv
	DB 012H,0A3H,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 09040H,1020H,0
	DB 'BT',0,0

	; PUSH GS
	DB 014H,0A8H,0FFH,00H
	DD 0
	DW 0720DH,0,0
	DB 'PUSH',0,0

	; POP GS
	DB 012H,0A9H,0FFH,00H
	DD 0
	DW 0720DH,0,0
	DB 'POP',0

	; SHRD Ev Gv Ib
	DB 014H,0ACH,0FFH,00H
	DD 00018010H			; MODRM+SIZEQ+DATA1
	DW 09040H,1020H,0E100H
	DB 'SHRD',0,0

	; SHRD Ev Gv CL
	DB 014H,0ADH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 09040H,1020H,01109H
	DB 'SHRD',0,0

	; IMUL Gv Ev
	DB 014H,0AFH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H,09040H,0
	DB 'IMUL',0,0

	; CMPXCHG Ev Gv
	DB 016H,0B0H,0FEH,00H
	DD 00018100H			; MODRM+SIZEQ+BITW
	DW 09040H,01020H,0
	DB 'CMPXCHG',0

	; LSS Gv Md
	DB 012H,0B2H,0FFH,00H
	DD 00010000H
	DW 01020H,09440H,0
	DB 'LSS',0

	; BTR Ev Gv
	DB 012H,0B3H,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 09040H,01020H,0
	DB 'BTR',0

	; LFS Gv Md
	DB 012H,0B4H,0FFH,00H
	DD 00010000H
	DW 01020H,09440H,0
	DB 'LFS',0

	; LGS Gv Md
	DB 012H,0B5H,0FFH,00H
	DD 00010000H
	DW 01020H,09440H,0
	DB 'LGS',0

	; MOVZX Gv Eb
	DB 014H,0B6H,0FFH,00H
	DD 00010000H			; MODRM
	DW 01020H,09140H,0
	DB 'MOVZX',0

	; MOVZX Gv Ew
	DB 014H,0B7H,0FFH,00H
	DD 00010000H			; MODRM
	DW 01020H,09240H,0
	DB 'MOVZX',0

	; BT Ev Ib
	DB 012H,0BAH,0FFH,04H		; Group index 4
	DD 00010010H			; MODRM+DATA1
	DW 09040H,0E100H,0
	DB 'BT',0,0

	; BTS Ev Ib
	DB 012H,0BAH,0FFH,05H		; Group index 5
	DD 00010010H			; MODRM+DATA1
	DW 09040H,0E100H,0
	DB 'BTS',0

	; BTR Ev Ib
	DB 012H,0BAH,0FFH,06H		; Group index 6
	DD 00010010H			; MODRM+DATA1
	DW 09040H,0E100H,0
	DB 'BTR',0

	; BTC Ev Ib
	DB 012H,0BAH,0FFH,07H		; Group index 7
	DD 00010010H			; MODRM+DATA1
	DW 09040H,0E100H,0
	DB 'BTC',0

	; BTC Ev Gv
	DB 012H,0BBH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 09040h,01020h,0
	DB 'BTC',0

	; BSF Gv Ev
	DB 012H,0BCH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H,09040H,0
	DB 'BSF',0

	; BSR Gv Ev
	DB 012H,0BDH,0FFH,00H
	DD 00018000H			; MODRM+SIZEQ
	DW 01020H,09040H,0
	DB 'BSF',0

	; MOVSX Gv Eb
	DB 014H,0BEH,0FFH,00H
	DD 00010000H			; MODRM
	DW 01020H,09140H,0
	DB 'MOVSX',0

	; MOVSX Gv Ew
	DB 014H,0BFH,0FFH,00H
	DD 00010000H			; MODRM
	DW 01020H,09240H,0
	DB 'MOVSX',0

	; XADD Ev Gv
	DB 014H,0C0H,0FEH,00H
	DD 00018100H			; MODRM+SIZEQ+BITW
	DW 09040H,01020H,0
	DB 'XADD',0,0

	; CMPXCHG8B
	DB 018H,0C7H,0FFH,01H		; Group index 1
	DD 00030000H
	DW 08840H,0,0
	DB 'CMPXCHG8B',0

	; BSWAP Gv
	DB 014H,0C8H,0F8H,00H
	DD 0
	DW 01010H,0,0
	DB 'BSWAP',0

	; MOVD MMx Ev
	DB 014H,06EH,0FFH,00H
	DD 00010000H			; MODRM
	DW 05820H,09440H,0
	DB 'MOVD',0


	; End of table signature
	DB 0FFH,0FFH
END_OF_TABLE:


end