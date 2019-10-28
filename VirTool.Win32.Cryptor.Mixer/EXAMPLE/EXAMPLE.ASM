
; CODEMIXER usage example.
; action: mix 2 code buffers and execute resulting buffer

callW                   macro   x
                        extern  x:PROC
                        call    x
                        endm

pusho                   macro   abc     ; push offset of the procedure 'abc'
                        local   __t
                        call    __t
                        jmp     abc
                        __t:
                        endm

                        p386
                        model   flat
                        locals  __
                        jumps

                        .data

randseed                dd      ?

c1:
                        int 3

                        xor     eax, eax
                        jz      __skip

                        callW   ReadFile        ; external call

                        mov     ecx, 00001234h  ; decryptor
                        mov     esi, 10000000h
__cycle:                xor     [esi], dl
                        inc     esi
                        dec     ecx
                        jnz     __cycle
                        inc     ecx

__skip:

c1end:

c2:
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        nop
                        callW   WriteFile       ; external call
                        nop
                        nop
c2end:

ldetbl                  db      2048 dup (?)

obufsize                dd      ?
obuf                    db      8192 dup (?)

                        .code

start:
                        push    offset ldetbl   ; initialize LDE32
                        call    disasm_init

                        ;;

                        pusho   my_disasm
                        pusho   my_rnd
                        push    offset c1
                        push    c1end-c1
                        push    offset c2
                        push    c2end-c2
                        push    offset obuf
                        push    size obuf
                        push    offset obufsize
                        push    1024
                        call    codemixer

                        lea     eax, obuf
                        mov     ebx, obufsize
                        mov     byte ptr [eax+ebx], 0C3h
                        call    eax

                        push    -1
                        callW   ExitProcess

include                 rnd.inc

include                 lde32bin.inc

my_disasm:              push    dword ptr [esp+4]
                        push    offset ldetbl
                        call    disasm_main             ; lde32bin.inc
                        ret

include                 ..\SRC\cmix.inc

                        end     start
