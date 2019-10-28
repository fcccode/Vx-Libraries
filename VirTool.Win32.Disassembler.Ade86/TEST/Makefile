ade = ../debug/ade86.exe
asm_test: $(ade)
	@echo IMM instructions
	@$(ade) imm.asm imm.out
	@fc /b  imm.bin imm.out

	@echo MODRM 16bit addressig
	@$(ade) m16.asm m16.out
	@fc /b  m16.bin m16.out

	@echo MODRM 32bit addressig
	@$(ade) m32.asm m32.out
	@fc /b  m32.bin m32.out

	@echo SIB addressig
	@$(ade) sib.asm sib.out
	@fc /b  sib.bin sib.out

	@echo Call/Jmp/Jcc instructions
	@$(ade) jmp.asm jmp.out
	@fc /b  jmp.bin jmp.out

	@echo All other instructions
	@$(ade) all.asm all.out
	@fc /b  all.bin all.out	
		
	@echo Assembler tested successful!

clean:
	del /f /q *.out
	del /f /q *.disout
