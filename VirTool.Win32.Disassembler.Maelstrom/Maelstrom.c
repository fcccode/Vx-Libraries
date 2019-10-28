/*
    .:: Maelstrom ::.


    fAMINE [ 2008 ]
    x86 IA32 Intel Disassembler

    1024 byte flag table
    Single handler function
    Struct output
    Minimal API nonsense

*/

#include <windows.h> // HeapAlloc, GetProcessHeap

#include "Maelstrom.h"

WORD 
FlagTable[512]=
{   
    MAELSTROM_FLAG_MODRM, // 00h: add r/m8, r8
    MAELSTROM_FLAG_MODRM, // 01h: add r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 02h: add r8, r/m8
    MAELSTROM_FLAG_MODRM, // 03h: add r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 04h: add al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 05h: add ax/eax, imm16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 06h: push es
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 07h: pop es
    MAELSTROM_FLAG_MODRM, // 08h: or r/m8, r8
    MAELSTROM_FLAG_MODRM, // 09h: or r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 0Ah: or r8, r/m8
    MAELSTROM_FLAG_MODRM, // 0Bh: or r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 0Ch: or al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 0Dh: or ax/eax, imm16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 0Eh: push cs
    MAELSTROM_FLAG_PREFIX_LOCK, // 0Fh: now combined with main table
    MAELSTROM_FLAG_MODRM, // 10h: adc r/m8, r8
    MAELSTROM_FLAG_MODRM, // 11h: adc r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 12h: adc r8, r/m8
    MAELSTROM_FLAG_MODRM, // 13h: adc r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 14h: adc al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 15h: adc ax/eax. imm16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 16h: push ss
    0, // 17h
    MAELSTROM_FLAG_MODRM, // 18h: sbb r/m8, r8
    MAELSTROM_FLAG_MODRM, // 19h: sbb r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 1Ah: sbb r8, r/m8
    MAELSTROM_FLAG_MODRM, // 1Bh: sbb r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 1Ch: sbb al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 1Dh: sbb ax/eax, imm16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 1Eh: push ds
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG2 + MAELSTROM_FLAG_SINGLE, // 1Fh: pop ds
    MAELSTROM_FLAG_MODRM, // 20h: and r/m8, r8
    MAELSTROM_FLAG_MODRM, // 21h: and r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 22h: and r8, r/m8
    MAELSTROM_FLAG_MODRM, // 23h: and r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 24h: and al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 25h: and ax/eax, imm16/32
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 26h
    0, // 27h: daa
    MAELSTROM_FLAG_MODRM, // 28h: sub r/m8, r8
    MAELSTROM_FLAG_MODRM, // 29h: sub r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 2Ah: sub r8, r/m8
    MAELSTROM_FLAG_MODRM, // 2Bh: sub r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 2Ch: sub al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 2Dh: sub ax/eax, imm16/32
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 2Eh
    0, // 2Fh: das
    MAELSTROM_FLAG_MODRM, // 30h: xor r/m8, r8
    MAELSTROM_FLAG_MODRM, // 31h: xor r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 32h: r8, r/m8
    MAELSTROM_FLAG_MODRM, // 33h: xor r16/32, r/m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 34h: xor al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 35h: xor ax/eax, imm16/32
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 36h
    0, // 37h: aaa
    MAELSTROM_FLAG_MODRM, // 38h: cmp r/m8, r8
    MAELSTROM_FLAG_MODRM, // 39h: cmp r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 3Ah: cmp r8, r/m8
    MAELSTROM_FLAG_MODRM, // 3Bh: cmp r16/32, m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 3Ch: cmp al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 3Dh: cmp ax/eax, imm16/33
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 3Eh
    0, // 3Fh: aas
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 40h: inc r16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 41h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 42h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 43h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 44h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 45h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 46h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 47h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 48h: dec r16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 49h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Ah
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Bh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Ch
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Dh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Eh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 4Fh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 50h: push r16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 51h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 52h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 53h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 54h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 55h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 56h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 57h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 58h: pop r16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 59h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Ah
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Bh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Ch
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Dh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Eh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 5Fh
    0, // 60h: pusha/pushad
    0, // 61h: popa/popad
    MAELSTROM_FLAG_MODRM, // 62h: bound r16/32, m16/32&16/32
    MAELSTROM_FLAG_MODRM, // 63h: arpl r/m16, r16
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 64h
    MAELSTROM_FLAG_PREFIX_SEGMENT, // 65h
    MAELSTROM_FLAG_PREFIX_OPERAND, // 66h
    MAELSTROM_FLAG_PREFIX_ADDRESS, // 67h
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // 68h: push imm16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE4, // 69h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // 6Ah: push imm8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // 6Bh
    0, // 6Ch: ins m8, dx or insb
    0, // 6Dh: ins m16/32, dx or insd
    0, // 6Eh: outs dx, m8 or outsb
    0, // 6Fh: out dx, m16/32 or outsw or outsd
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 70h: jo rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 71h: jno rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 72h: jb/jc/jnae rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 73h: jae/jnc rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 74h: je/jz rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 75h: jnz/jne rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 76h: jbe/jna rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 77h: ja/jnbe rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 78h: js rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 79h: jns rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Ah: jp/jpe rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Bh: jpo/jnp rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Ch: jnge/jl rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Dh: jge/jnl rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Eh: jng/jle rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // 7Fh: jg/jnle rel8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // 80h: r/m8, imm8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE4, // 81h: r/m16/32, imm16/32
    0, // 82h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SIGNEXTEND, // 83h: r/m16/32, imm8
    MAELSTROM_FLAG_MODRM, // 84h: test r/m8, r8
    MAELSTROM_FLAG_MODRM, // 85h: test r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 86h: xchg r/m8, r8
    MAELSTROM_FLAG_MODRM, // 87h: r16/32, r/m16/32
    MAELSTROM_FLAG_MODRM, // 88h: mov r/m8, r8
    MAELSTROM_FLAG_MODRM, // 89h: mov r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // 8Ah: mov r8, r/m8
    MAELSTROM_FLAG_MODRM, // 8Bh: mov r16/32 r/m16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG3, // 8Ch: mov r/m16, sreg
    MAELSTROM_FLAG_MODRM, // 8Dh: lea r16/32, m
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SEGMENTREG3, // 8Eh: mov sreg, r/m16/32
    MAELSTROM_FLAG_MODRM, // 8Fh: pop m16/32
    0, // 90h: nop
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 91h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 92h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 93h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 94h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 95h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 96h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // 97h
    0, // 98h: cbw
    0, // 99h: cwde
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_IMMEDIATE4, // 9Ah: call ptr16:16/32
    0, // 9Bh: finit/wait/fwait/fsave/fnsave
    0, // 9Ch: pushf/pushfd
    0, // 9Dh: popf/popfd
    0, // 9Eh: sahf
    0, // 9Fh: lahf
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // A0h: mov al, moffs8
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // A1h: mov ax, moffs16
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // A2h: mov moffs8, al
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // A3h: mov moffs16/32, ax/eax
    0, // A4h: movs m8, m8
    0, // A5h: movs m16/32, m16/32
    0, // A6h: cmps m8, m8
    0, // A7h: cmps m16/32, m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // A8h: test al, imm8
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // A9h: test ax/eax, imm16/32
    0, // AAh: stos m8
    0, // ABh: stos m16/32
    0, // ACh: lods m8
    0, // ADh: lods m16/32
    0, // AEh: scas m8
    0, // AFh: scas m16/32
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B0h: mov r8, imm8
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B1h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B2h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B3h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B4h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B5h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B6h
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // B7h
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // B8h: mov r16/32, imm16/32
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // B9h
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BAh
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BBh
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BCh
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BDh
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BEh
    MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE, // BFh
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // C0h: rcl/rcr/ror/sal/sar/shl/shr r/m8, imm8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SIGNEXTEND, // C1h: rcl/rcr/ror/sal/sar/shl/shr r/m16/32, imm8
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE + MAELSTROM_FLAG_PREFIX_ADDRESS, // C2h: ret imm16
    0, // C3h: ret
    MAELSTROM_FLAG_MODRM, // C4h: les r16/32:m:16/32
    MAELSTROM_FLAG_MODRM, // C5h: lds r16/32:m:16/32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // C6h: mov r/m8, imm8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE4, // C7h: mov r/m16/32, imm16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE + MAELSTROM_FLAG_PREFIX_ADDRESS, // C8h: enter imm16, imm8
    0, // C9h: leave
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE + MAELSTROM_FLAG_PREFIX_ADDRESS, // CAh: ret imm16
    0, // CBh: ret
    0, // CCh: int3
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // CDh: int imm8
    0, // CEh: into
    0, // CFh: iret
    MAELSTROM_FLAG_MODRM, // D0h: rcl/rcr/ror/sal/sar/shl/shr r/m8, 1
    MAELSTROM_FLAG_MODRM, // D1h: rcl/rcr/ror/sal/sar/shl/shr r/m16/32, 1
    MAELSTROM_FLAG_MODRM, // D2h: rcl/rcr/ror/sal/sar/shl/shr r/m8, cl
    MAELSTROM_FLAG_MODRM, // D3h: rcl/rcr/ror/sal/sar/shl/shr r/m16, cl
    0, // D4h: aam
    0, // D5h: aad
    0, // D6h
    0, // D7h: xlat m8
    0, // D8h: fpu
    0, // D9h: fpu
    0, // DAh: fpu
    0, // DBh: fpu
    0, // DCh: fpu
    0, // DDh: fpu
    0, // DEh: fpu
    0, // DFh: fpu
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // E0h: loopnz rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // E1h: loopz rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // E2h: loop rel8
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // E3h: jcxz
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // E4h: in al, imm8
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // E5h: in ax/eax, imm8
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // E6h: out imm8, al
    MAELSTROM_FLAG_IMMEDIATE1 + MAELSTROM_FLAG_SINGLE, // E7h: out imm8, ax/eax
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // E8h: call rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // E9h: rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_IMMEDIATE4 + MAELSTROM_FLAG_SINGLE + MAELSTROM_FLAG_PREFIX_OPERAND, // EAh: ptr16:16/32
    MAELSTROM_FLAG_DISPLACEMENT1 + MAELSTROM_FLAG_SINGLE, // EBh: jmp rel8
    0, // ECh: in al, dx
    0, // EDh: in ax/eax, dx
    0, // EEh: out dx, al
    0, // EFh: out dx, ax/eax
    MAELSTROM_FLAG_PREFIX_LOCK, // F0h: lock signal prefix
    0, // F1h
    MAELSTROM_FLAG_PREFIX_REP, // F2h: string prefix
    MAELSTROM_FLAG_PREFIX_REP, // F3h: string prefix
    0, // F4h: hlt
    0, // F5h: cmc
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // F6h: div r/m8
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE4, // F7h: div r/m16/32
    0, // F8h: clc
    0, // F9h: stc
    0, // FAh: cli
    0, // FBh: sti
    0, // FCh: cld
    0, // FDh: std
    MAELSTROM_FLAG_MODRM, // FEh
    MAELSTROM_FLAG_MODRM, // FFh
    
    // And so begins the 0F table...
    
    MAELSTROM_FLAG_MODRM, // 00h
    MAELSTROM_FLAG_MODRM, // 01h
    MAELSTROM_FLAG_MODRM, // 02h: lar r16/32, r/m16/32
    MAELSTROM_FLAG_MODRM, // 03h: lsl r16/32, r/m16/32
    0, // 04h
    0, // 05h
    0, // 06h: clts
    0, // 07h
    0, // 08h: invd
    0, // 09h: wbinvd
    0, // 0Ah
    0, // 0Bh
    0, // 0Ch
    0, // 0Dh
    0, // 0Eh
    0, // 0Fh
    0, // 10h
    0, // 11h
    0, // 12h
    0, // 13h
    0, // 14h
    0, // 15h
    0, // 16h
    0, // 17h
    0, // 18h
    0, // 19h
    0, // 1Ah
    0, // 1Bh
    0, // 1Ch
    0, // 1Dh
    0, // 1Eh
    0, // 1Fh
    MAELSTROM_FLAG_MODRM, // 20h: mov r32, cr0/cr2/cr3/cr4
    MAELSTROM_FLAG_MODRM, // 21h: mov r32, dr0/dr1/dr2/dr3/dr4/dr5/dr6/dr7
    MAELSTROM_FLAG_MODRM, // 22h: mov cr0/cr2/cr3/cr4, r32
    MAELSTROM_FLAG_MODRM, // 23h: mov dr0/dr1/dr2/dr3/dr4/dr5/dr6/dr7, r32
    0, // 24h
    0, // 25h
    0, // 26h
    0, // 27h
    0, // 28h
    0, // 29h
    0, // 2Ah
    0, // 2Bh
    0, // 2Ch
    0, // 2Dh
    0, // 2Eh
    0, // 2Fh
    0, // 30h: wrmsr
    0, // 31h
    0, // 32h: rdmsr
    0, // 33h
    0, // 34h
    0, // 35h
    0, // 36h
    0, // 37h
    0, // 38h
    0, // 39h
    0, // 3Ah
    0, // 3Bh
    0, // 3Ch
    0, // 3Dh
    0, // 3Eh
    0, // 3Fh
    0, // 40h
    0, // 41h
    0, // 42h
    0, // 43h
    0, // 44h
    0, // 45h
    0, // 46h
    0, // 47h
    0, // 48h
    0, // 49h
    0, // 4Ah
    0, // 4Bh
    0, // 4Ch
    0, // 4Dh
    0, // 4Eh
    0, // 4Fh
    0, // 50h
    0, // 51h
    0, // 52h
    0, // 53h
    0, // 54h
    0, // 55h
    0, // 56h
    0, // 57h
    0, // 58h
    0, // 59h
    0, // 5Ah
    0, // 5Bh
    0, // 5Ch
    0, // 5Dh
    0, // 5Eh
    0, // 5Fh
    0, // 60h
    0, // 61h
    0, // 62h
    0, // 63h
    0, // 64h
    0, // 65h
    0, // 67h
    0, // 68h
    0, // 69h
    0, // 6Ah
    0, // 6Bh
    0, // 6Ch
    0, // 6Dh
    0, // 6Eh
    0, // 6Fh
    0, // 70h
    0, // 71h
    0, // 72h
    0, // 73h
    0, // 74h
    0, // 75h
    0, // 76h
    0, // 77h
    0, // 78h
    0, // 79h
    0, // 7Ah
    0, // 7Bh
    0, // 7Ch
    0, // 7Dh
    0, // 7Eh
    0, // 7Fh
    0, // 80h
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 81h: jno
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 82h: jb/jc
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 83h: jae/jnb
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 84h: je/jz rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 85h: jnz/jne rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 86h: jbe/jna rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 87h: ja/jnbe rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 88h: js rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 89h: jns rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 8Ah: jp/jpe rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 8Bh: jnp/jpo rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 8Ch: jnge/jl rel16/32
    MAELSTROM_FLAG_DISPLACEMENT4 + MAELSTROM_FLAG_SINGLE, // 8Dh: jge/jnl rel16/32
    0, // 8Eh
    MAELSTROM_FLAG_MODRM, // 90h: seto r/m8
    MAELSTROM_FLAG_MODRM, // 91h: setnp r/m8
    MAELSTROM_FLAG_MODRM, // 92h: setb/setc/setnae r/m8
    MAELSTROM_FLAG_MODRM, // 93h: setae/setnb/setnc r/m8
    MAELSTROM_FLAG_MODRM, // 94h: sete/setz r/m8
    MAELSTROM_FLAG_MODRM, // 95h: setne/setnz r/m8
    MAELSTROM_FLAG_MODRM, // 96h: setbe/setna r/m8
    MAELSTROM_FLAG_MODRM, // 97h: seta/setnbe r/m8
    MAELSTROM_FLAG_MODRM, // 98h: sets r/m8
    MAELSTROM_FLAG_MODRM, // 99h: setns r/m8
    MAELSTROM_FLAG_MODRM, // 9Ah: setp r/m8
    MAELSTROM_FLAG_MODRM, // 9Bh: setnp/setpo r/m8
    MAELSTROM_FLAG_MODRM, // 9Ch: setl/setnge r/m8
    MAELSTROM_FLAG_MODRM, // 9Dh: setge/setnl r/m8
    MAELSTROM_FLAG_MODRM, // 9Eh: setle/setng r/m8
    MAELSTROM_FLAG_MODRM, // 9Fh: setg/setnle r/m8
    0, // A0h
    0, // A1h
    0, // A2h: cpuid
    MAELSTROM_FLAG_MODRM, // A3h: bt r/m16/32, r16/32
    0, // A4h
    0, // A5h
    0, // A6h
    0, // A7h
    0, // A8h
    0, // A9h
    0, // AAh: rsm
    MAELSTROM_FLAG_MODRM, // ABh: bts r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // ACh: shrd r/m16/32, r16/32, imm8
    MAELSTROM_FLAG_MODRM, // ADh: shrd r/m16/32, r16/32, cl
    0, // AEh
    0, // AFh
    MAELSTROM_FLAG_MODRM, // B0h: cmpxchg r/m8, r8
    MAELSTROM_FLAG_MODRM, // B1h: cmpxchg r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // B2h: lss r16/32, m16:16/32
    MAELSTROM_FLAG_MODRM, // B3h: btr r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // B4h: lfs r16/32, m16:16/32
    MAELSTROM_FLAG_MODRM, // B5h: lgs r16/32, m16:16/32
    MAELSTROM_FLAG_MODRM, // B6h: movzx r16/32, r/m8
    MAELSTROM_FLAG_MODRM, // B7h: movzx r32, r/m16
    0, // B8h
    0, // B9h
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_IMMEDIATE1, // BAh: bt/btr/btc/bts r/m16/32, imm8
    MAELSTROM_FLAG_MODRM, // BBh: btc r/m16/32, r16/32
    MAELSTROM_FLAG_MODRM, // BCh: bsf r16/32, r/m16/32
    MAELSTROM_FLAG_MODRM, // BDh: bsr r16/32, r/m16/32
    MAELSTROM_FLAG_MODRM, // BEh: movsx r16/32, r/m8
    MAELSTROM_FLAG_MODRM, // BFh: movsx r32, r/m16
    MAELSTROM_FLAG_MODRM, // C0h: xadd r/m8, r8
    MAELSTROM_FLAG_MODRM, // C1h: xadd r/m16/32, r16/32
    0, // C2h
    0, // C3h
    0, // C4h
    0, // C5h
    0, // C6h
    MAELSTROM_FLAG_MODRM, // C7h: cmpxchg8b r/m64
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // C8h: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // C9h: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CAh: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CBh: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CCh: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CDh: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CEh: bswap r32
    MAELSTROM_FLAG_MODRM + MAELSTROM_FLAG_SINGLE, // CFh: bswap r32
    0, // D0h
    0, // D1h
    0, // D2h
    0, // D3h
    0, // D4h
    0, // D5h
    0, // D6h
    0, // D7h
    0, // D8h
    0, // D9h
    0, // DAh
    0, // DBh
    0, // DCh
    0, // DEh
    0, // DFh
    0, // E0h
    0, // E1h
    0, // E2h
    0, // E3h
    0, // E4h
    0, // E5h
    0, // E6h
    0, // E7h
    0, // E8h
    0, // E9h
    0, // EAh
    0, // EBh
    0, // ECh
    0, // EEh
    0, // EFh
    0, // F0h
    0, // F1h
    0, // F2h
    0, // F3h
    0, // F4h
    0, // F5h
    0, // F6h
    0, // F7h
    0, // F8h
    0, // F9h
    0, // FAh
    0, // FBh
    0, // FCh
    0, // FEh
    0, // FFh
};

VOID
MaelstromInstructionHandler
(PIA32_X86_INSTRUCTION pInstruction, PCHAR *pCodeBlock)
{   
    // The actual handler code: parses Modrm, buffers Immediate/Displacement data
    
    DWORD dwTableOffset;
    
    _maelstrom_routine_start:
                             
    dwTableOffset=**pCodeBlock;
                                
    if(**pCodeBlock == '\x0F') 
    {
         *pCodeBlock++;
         dwTableOffset += 256;
    }

    pInstruction->Flags  |= FlagTable[dwTableOffset];
	
    pInstruction->Opcode=**pCodeBlock;

    *pCodeBlock++;
	
    if((pInstruction->Flags & MAELSTROM_FLAG_PREFIX_ANY)) goto _maelstrom_routine_start;
	
    // Abnormal cases
	
	if((**pCodeBlock & '\x1C')) // // Normally these are plain Modrm, however for Test they are Modrm|Immediate
	{
        if(pInstruction->Opcode == '\xF6') pInstruction->Flags &= ~MAELSTROM_FLAG_IMMEDIATE1;
        else if(pInstruction->Opcode == '\xF7') pInstruction->Flags &= ~MAELSTROM_FLAG_IMMEDIATE4;
    }
    
    if((pInstruction->Flags & MAELSTROM_FLAG_SINGLE)) *pCodeBlock--; // Cancel out increment
    if((pInstruction->Flags & MAELSTROM_FLAG_DOUBLE)) *pCodeBlock++; // Two byte non-prefixed opcodes
    
    if((pInstruction->Flags & MAELSTROM_FLAG_MODRM))
    {
        pInstruction->Mod=(**pCodeBlock & '\xC0'); // 11000000
        pInstruction->Reg=(**pCodeBlock & '\x1C'); // 00111000
        pInstruction->Rm=(**pCodeBlock & '\x07'); // 00000111
        
        if(pInstruction->Mod != '\xC0' && (pInstruction->Flags & MAELSTROM_FLAG_SINGLE) == '\x00') // 11000000
        {
            // Displacement data will be used
         
            if(pInstruction->Rm == '\x04')
            {
                 pInstruction->Flags |= MAELSTROM_FLAG_SIB;
                
                 pInstruction->Sib=*(*pCodeBlock++);
            }
         
            if((pInstruction->Rm == '\x05' && pInstruction->Mod == '\x00') || pInstruction->Mod == '\x80') // 10000000
            {
                 pInstruction->Flags |= MAELSTROM_FLAG_DISPLACEMENT4;
            }
         
            if(pInstruction->Mod == '\x40') // 01000000
            {
                 pInstruction->Flags |= MAELSTROM_FLAG_DISPLACEMENT1;
            }
        } 
        // Else it is a regreg, direct modrm reg, or single byte reg
    }  
    
    _maelstrom_routine_parse_data:
    
    CHAR cDataSize;
    
    cDataSize=(pInstruction->Flags & (MAELSTROM_FLAG_DISPLACEMENT_ANY));
    cDataSize += ((pInstruction->Flags & (MAELSTROM_FLAG_IMMEDIATE_ANY)) >> 1);
    
    if((pInstruction->Flags & MAELSTROM_FLAG_PREFIX_ADDRESS) || (pInstruction->Flags & MAELSTROM_FLAG_PREFIX_OPERAND))
    {
        cDataSize >>= 1;
    }
    
    CHAR cIndex;
    
    for(cIndex='\x00'; cIndex < cDataSize; cIndex++)
    {
        pInstruction->Data[cIndex]=**pCodeBlock++; // If the size is greater than 4, it will bleed into the immediate buffer
    }
    
    return;
}  
   
PIA32_X86_INSTRUCTION
MaelstromMainRoutine
(PCHAR pCodeBlock, DWORD dwCodeBlockSize)
{  
    // Routine will build the linked list. Parameters represent a .text segment and its size.
   
    DWORD dwBytesProcessed;
    PCHAR pCodeBlockOffset;
    PIA32_X86_INSTRUCTION pHeadInstruction, pInstruction, pNextInstruction; 

    dwBytesProcessed=0; 

    pCodeBlockOffset=pCodeBlock;
 
    pHeadInstruction=pInstruction=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IA32_X86_INSTRUCTION));

    while(dwBytesProcessed < dwCodeBlockSize)
    {
        pNextInstruction=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IA32_X86_INSTRUCTION));
       
        pNextInstruction->Flink=NULL;
        pNextInstruction->Blink=pInstruction;
 
        pInstruction->Flink=pNextInstruction;
 	   
        MaelstromInstructionHandler(pInstruction,&pCodeBlockOffset);
 	   
        pInstruction=pNextInstruction;
        pInstruction->Size=(pCodeBlockOffset - pCodeBlock);
        dwBytesProcessed += pInstruction->Size;
    }

    return(pHeadInstruction);
}
