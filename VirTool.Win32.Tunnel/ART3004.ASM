;; * v … 0.04 (the tunneler + surface engine formerly known as ART v3.0)
ideal ; use TASM - you'll like yourself more
segment code 'code'
assume cs:code,ds:code,es:code,ss:code
radix 16 ; use radix 16 or the results will be BIZARRE
org 100 ; this is a .com- doesn't have to be though
P286
start:
push sp
pop ax
cmp ax,sp
je ok_286
push dx
db 0f ; pop cs (on 8086 :)
ok_286:
mov [word ptr cs:save_ret_to],initialize-after_save
mov ah,30
jmp save_regs
initialize:
mov ah,52
int 21
mov ax,[es:bx-2]
mov [firstMCB],ax
mov [word ptr cs:save_ret_to],restore_proc_state-after_save
call far ztart
the_int:
;include "testops.inc"
cli
int 21
sti
push cs
pop ds
xor ax,ax
cmp ax,[tunnel_cs]
jne successful
mov ah,9
mov dx,offset unsuc_msg
int 21
mov ax,4c00
int 21
successful:
mov ah,9
mov dx,offset suc_msg
int 21
mov ax,4c00
int 21
unsuc_msg:
db 'Did not work :($' ; tells you if it works or not
suc_msg:
db 'Worked :)$'
ztart:
xor cx,cx
pop si ds
jmp past_trampoline ; you jump off of a trampoline :)
jump_table:
db ModRM ; 0 add r/m8,r8
db ModRM ; 1 add r/m16,r16
db ModRM ; 2 add r8,r/m8
db ModRM ; 3 add r16,r/m16
db two_bytes ; 4 add al,xx
db maybe_5_bytes ; 5 add ax,xxxx(add eax,xxxxxxxx)
db one_byte ; 6 push es
db one_byte ; 7 pop es
db ModRM ; 8 or r/m8,r8
db ModRM ; 9 or r/m16,r16
db ModRM ; a or r8,r/m8
db ModRM ; b or r16,r/m16
db two_bytes ; c or al,xx
db maybe_5_bytes ; d or ax,xxxx(or eax,xxxxxxxx)
db push_cs ; e push cs
db escape ; f escape opcode
db ModRM ; 10 adc r/m8,r8
db ModRM ; 11 adc r/m16,r16
db ModRM ; 12 adc r8,r/m8
db ModRM ; 13 adc r16,r/m16
db two_bytes ; 14 adc al,xx
db maybe_5_bytes ; 15 adc ax,xxxx(adc eax,xxxxxxxx)
db one_byte ; 16 push ss
db one_byte ; 17 pop ss
db ModRM ; 18 sbb r/m8,r8
db ModRM ; 19 sbb r/m16,r16
db ModRM ; 1a sbb r8,r/m8
db ModRM ; 1b sbb r16,r/m16
db two_bytes ; 1c sbb al,xx
db maybe_5_bytes ; 1d sbb ax,xxxx(sbb eax,xxxxxxxx)
db push_ds ; 1e push ds
db one_byte ; 1f pop ds
db ModRM ; 20 and r/m8,r8
db ModRM ; 21 and r/m16,r16
db ModRM ; 22 and r8,r/m8
db ModRM ; 23 and r16,r/m16
db two_bytes ; 24 and al,xx
db maybe_5_bytes ; 25 and ax,xxxx(and eax,xxxxxxxx)
db override ; 26 es:
db one_byte ; 27 daa
db ModRM ; 28 sub r/m8,r8
db ModRM ; 29 sub r/m16,r16
db ModRM ; 2a sub r8,r/m8
db ModRM ; 2b sub r16,r/m16
db two_bytes ; 2c sub al,xx
db maybe_5_bytes ; 2d sub ax,xxxx(sub eax,xxxxxxxx)
db cs_override ; 2e cs:
db one_byte ; 2f das
db ModRM ; 30 xor r/m8,r8
db ModRM ; 31 xor r/m16,r16
db ModRM ; 32 xor r8,r/m8
db ModRM ; 33 xor r16,r/m16
db two_bytes ; 34 xor al,xx
db maybe_5_bytes ; 35 xor ax,xxxx(xor eax,xxxxxxx)
db override ; 36 ss:
db one_byte ; 37 aaa
db ModRM ; 38 cmp r/m8,r8
db ModRM ; 39 cmp r/m16,r16
db ModRM ; 3a cmp r8,r/m8
db ModRM ; 3b cmp r16,r/m16
db two_bytes ; 3c cmp al,xx
db maybe_5_bytes ; 3d cmp ax,xxxx(cmp eax,xxxxxxxx)
db override ; 3e ds:
db one_byte ; 3f aas
db one_byte ; 40 inc ax
db one_byte ; 41 inc cx
db one_byte ; 42 inc dx
db one_byte ; 43 inc bx
db one_byte ; 44 inc sp
db one_byte ; 45 inc bp
db one_byte ; 46 inc si
db one_byte ; 47 inc di
db one_byte ; 48 dec ax
db one_byte ; 49 dec cx
db one_byte ; 4a dec dx
db one_byte ; 4b dec bx
db one_byte ; 4c dec sp
db one_byte ; 4d dec bp
db one_byte ; 4e dec si
db one_byte ; 4f dec di
db one_byte ; 50 push ax
db one_byte ; 51 push cx
db one_byte ; 52 push dx
db one_byte ; 53 push bx
db one_byte ; 55 push sp
db one_byte ; 55 push bp
db one_byte ; 56 push si
db one_byte ; 57 push di
db one_byte ; 58 pop ax
db one_byte ; 59 pop cx
db one_byte ; 5a pop dx
db one_byte ; 5b pop bx
db one_byte ; 5c pop sp
db one_byte ; 5d pop bp
db one_byte ; 5e pop si
db one_byte ; 5f pop di
db one_byte ; 60 pusha
db one_byte ; 61 popa
db bound_ ; 62 bound
db finish ; 63 arpl - windows jump point, usually
db override ; 64 fs:
db override ; 65 gs:
db op_size ; 66 operand size override - 386+
db adr_size ; 67 address size override - 386+
db maybe_5_bytes ; 68 push xxxx(push xxxxxxxx)
db ModRM_plus_2 ; 69 imul r16,r/m16,xxxx
db two_bytes ; 6a push sign-extended xx as word
db ModRM_plus_1 ; 6b imul r8,r/m8,xx
db one_byte ; 6c insb
db one_byte ; 6d insw
db one_byte ; 6e outsb
db one_byte ; 6f outsw
db cond_jump ; 70 jo
db cond_jump ; 71 jno
db cond_jump ; 72 jb
db cond_jump ; 73 jae
db cond_jump ; 74 jz
db cond_jump ; 75 jnz
db cond_jump ; 76 jbe
db cond_jump ; 77 ja
db cond_jump ; 78 js
db cond_jump ; 79 jns
db cond_jump ; 7a jp
db cond_jump ; 7b jnp
db cond_jump ; 7c jl
db cond_jump ; 7d jg
db cond_jump ; 7e jle
db cond_jump ; 7f jge
db ModRM_plus_1 ; 80 immediate word opcodes xx
db ModRM_plus_2 ; 81 immediate word opcodes xxxx
db ModRM_plus_1 ; 82 immediate byte opcodes xx
db ModRM_plus_1 ; 83 immediate word opcodes sign-extended xx
db ModRM ; 84 test r/m8,r8
db ModRM ; 85 test r/m16,r16
db ModRM ; 86 xchg r/m8,r8
db ModRM ; 87 xchg r/m16,r16
db ModRM ; 88 mov r/m8,r8
db ModRM ; 89 mov r/m16,r16
db ModRM ; 8a mov r8,r/m8
db ModRM ; 8b mov r16,r/m16
db mov_seg ; 8c mov r16,segreg
db ModRM ; 8d lea r16,r/m16
db ModRM ; 8e mov segreg,r16
db ModRM ; 8f pop r/m16
db one_byte ; 90 nop
db one_byte ; 91 xchg ax,cx
db one_byte ; 92 xchg ax,dx
db one_byte ; 93 xchg ax,bx
db one_byte ; 94 xchg ax,sp
db one_byte ; 95 xchg ax,bp
db one_byte ; 96 xchg ax,si
db one_byte ; 97 xchg ax,di
db one_byte ; 98 cbw
db one_byte ; 99 cwd
db far_call ; 9a call far xxxx:xxxx
db prefix ; 9b wait
db do_pushf ; 9c pushf
db do_popf ; 9d popf
db one_byte ; 9e sahf
db one_byte ; 9f lahf
db three_bytes ; a0 mov al,[xxxx]
db maybe_5_bytes2 ; a1 mov ax,[xxxx]
db maybe_5_bytes2 ; a2 mov [xxxx],al
db maybe_5_bytes2 ; a3 mov [xxxx],ax
db one_byte ; a4 movsb
db one_byte ; a5 movsw
db one_byte ; a6 cmpsb
db one_byte ; a7 cmpsw
db two_bytes ; a8 test al,xx
db maybe_5_bytes ; a9 test ax,xxxx(test eax,xxxxxxxx)
db one_byte ; aa stosb
db one_byte ; ab stosw
db one_byte ; ac lodsb
db one_byte ; ad lodsw
db one_byte ; ae scasb
db one_byte ; af scasw
db two_bytes ; b0 mov al,xx
db two_bytes ; b1 mov cl,xx
db two_bytes ; b2 mov dl,xx
db two_bytes ; b3 mov bl,xx
db two_bytes ; b4 mov ah,xx
db two_bytes ; b5 mov ch,xx
db two_bytes ; b6 mov dh,xx
db two_bytes ; b7 mov bh,xx
db maybe_5_bytes ; b8 mov ax,xxxx(mov eax,xxxxxxxx)
db maybe_5_bytes ; b9 mov dx,xxxx(mov edx,xxxxxxxx)
db maybe_5_bytes ; ba mov cx,xxxx(mov ecx,xxxxxxxx)
db maybe_5_bytes ; bb mov bx,xxxx(mov ebx,xxxxxxxx)
db maybe_5_bytes ; bc mov sp,xxxx(mov esp,xxxxxxxx)
db maybe_5_bytes ; bd mov bp,xxxx(mov ebp,xxxxxxxx)
db maybe_5_bytes ; be mov si,xxxx(mov esi,xxxxxxxx)
db maybe_5_bytes ; bf mov di,xxxx(mov edi,xxxxxxxx)
db ModRM_plus_1 ; c0 rol/ror/rcl/rcr/shl/shr/sar rm8,xx
db ModRM_plus_1 ; c1 rol/ror/rcl/rcr/shl/shr/sar rm16,xx
db ret_iw ; c2 ret iw
db return ; c3 ret
db ModRM ; c4 les rx,r/m16
db ModRM ; c5 lds rx,r/m16
db ModRM_plus_1 ; c6 mov r/m8,xx
db ModRM_plus_2 ; c7 mov r/m16,xxxx
db four_bytes ; c8 enter xxxx,xx
db one_byte ; c9 leave
db retf_iw ; ca retf iw
db returnf ; cb retf
db int3 ; cc int 3
db interrupt ; cd int xx
db int_ov ; ce into
db int_ret ; cf iret
db ModRM ; d0 rol/ror/rcl/rcr/shl/shr/sar r/m8,1
db ModRM ; d1 rol/ror/rcl/rcr/shl/shr/sar r/m16,1
db ModRM ; d2 rol/ror/rcl/rcr/shl/shr/sar r/m8,cl
db ModRM ; d3 rol/ror/rcl/rcr/shl/shr/sar r/m16,cl
db aam_ ; d4 aam
db two_bytes ; d5 aad
db abort ; d6 not an opcode
db one_byte ; d7 xlat
db ModRM ; d8 coprocessor command
db ModRM ; d9 coprocessor command
db ModRM ; da coprocessor command
db ModRM ; db coprocessor command
db ModRM ; dc coprocessor command
db ModRM ; dd coprocessor command
db ModRM ; de coprocessor command
db ModRM ; df coprocessor command
db loop_ne ; e0 loopne xx
db loop_e ; e1 loope xx
db do_loop ; e2 loop xx
db cond_jump ; e3 jcxz xx
db two_bytes ; e4 in al,xx
db two_bytes ; e5 in ax,xx
db two_bytes ; e6 out xx,al
db two_bytes ; e7 out xx,ax
db do_call ; e8 call xxxx
db jmp_near ; e9 jmp xxxx
db jmp_far ; ea jmp xxxx:xxxx
db jmp_short ; eb jmp xx
db one_byte ; ec in al,dx
db one_byte ; ed in ax,dx
db one_byte ; ee out dx,al
db one_byte ; ef out dx,ax
db prefix ; f0 lock
db bpice ; f1 bpice - generate int 1 (undocumented)
db prefix ; f2 repne
db prefix ; f3 repe
db one_byte ; f4 halt
db one_byte ; f5 cmc
db ModRM_plus_1_f6 ; f6 lotsa stuff r/m8 (test xx/xx,not,neg,mul,
db ModRM_plus_2_f7 ; f7 lotsa stuff r/m16 ...imul,div,idiv)
db one_byte ; f8 clc
db one_byte ; f9 stc
db one_byte ; fa cli
db one_byte ; fb sti
db one_byte ; fc cld
db one_byte ; fd std
db ModRM ; fe inc/dec r/m8
db ModRM_ff ; ff inc/dec r/m16/call r/m16/call far r/m16
;    jmp r/m16/jmp far r/m16/push r/m16
past_table:
;;////////////////////////////////////////////////////////////////////
_ModRM_plus_1_f6:
call swap_ints
test al,38
je _ModRM_plus_1
jmp _ModRM
;;////////////////////////////////////////////////////////////////////
_ModRM_plus_2_f7:
call swap_ints
test al,38
je _ModRM_plus_2
jmp _ModRM
;;////////////////////////////////////////////////////////////////////
_ModRM_ff:
test al,30
jpe _ModRM
jmp calc_ModRMff
;;////////////////////////////////////////////////////////////////////
_ModRM_plus_2:
jz ModRM_p4
js only_2_bytes
ModRM_p4:
movsw
only_2_bytes:
movsb
;;////////////////////////////////////////////////////////////////////
_ModRM_plus_1:
movsb ; use movsb in case opcode has been stored
;;////////////////////////////////////////////////////////////////////
_ModRM:
jmp do_ModRM
;;////////////////////////////////////////////////////////////////////
_bound:
call swap_ints
jmp _ModRM
;;////////////////////////////////////////////////////////////////////
_mov_seg:
jmp do_movseg
;;////////////////////////////////////////////////////////////////////
_push_cs:
or cl,10 ; set DS=CS before simulating
mov al,1e ; change instruction to PUSH DS
dec di
stosb
jmp setup_simulate
;;////////////////////////////////////////////////////////////////////
_escape:
jmp escape_opcode
;;////////////////////////////////////////////////////////////////////
_pushf:
call save_sssp
mov ax,[cs:flags]
or ah,ch
push ax
call restore_sssp
jmp past_trampoline1
;;////////////////////////////////////////////////////////////////////
_popf:
call save_sssp
pop ax
mov ch,1
and ch,ah
xor ah,ch
mov [cs:flags],ax
call restore_sssp
past_trampoline1:
jmp past_trampoline2
;;////////////////////////////////////////////////////////////////////
_loop_ne:
_loop_e:
_do_loop:
_cond_jump:
jmp do_cond_jump
;;////////////////////////////////////////////////////////////////////
_return:
xor ax,ax
;;////////////////////////////////////////////////////////////////////
_ret_iw:
jmp do_ret_iw
;;////////////////////////////////////////////////////////////////////
_returnf:
xor ax,ax
;;////////////////////////////////////////////////////////////////////
_retf_iw:
call save_sssp
pop si ds
add sp,ax
call restore_sssp
past_trampoline2:
jmp past_trampoline3
;;////////////////////////////////////////////////////////////////////
_int3:
mov al,3
cmp ax,0
org $-2
;;////////////////////////////////////////////////////////////////////
_bpice:
mov al,1
jmp do_int
;;////////////////////////////////////////////////////////////////////
_interrupt:
inc si
goto_int:
jmp do_int
;;////////////////////////////////////////////////////////////////////
_int_ov:
mov al,4
test [byte ptr cs:flags+1],8
jnz goto_int
past_trampoline3:
jmp past_trampoline4
;;////////////////////////////////////////////////////////////////////
_int_ret:
jmp do_int_ret
;;////////////////////////////////////////////////////////////////////
_jmp_short:
lodsb
cbw
jmp rest_of_jmp
;;////////////////////////////////////////////////////////////////////
_do_call:
lodsw ; add 2 to si
call save_sssp
push si ; 64k segment limit
call restore_sssp
dec si
dec si
;;////////////////////////////////////////////////////////////////////
_jmp_near:
lodsw
rest_of_jmp:
add si,ax ; 'execute' the jump
past_trampoline4:
jmp past_trampoline5
;;////////////////////////////////////////////////////////////////////
_far_call:
push si
lodsw
lodsw
call save_sssp
push ds si
call restore_sssp
pop si
;;////////////////////////////////////////////////////////////////////
_jmp_far:
mov ds,[si+2]
xchg ax,si
past_trampoline5:
jmp past_trampoline6
;;////////////////////////////////////////////////////////////////////
_cs_override:
dec di
or cl,10
mov al,3e ; change instruction to DS: from CS:
stosb
jmp _prefix
;;////////////////////////////////////////////////////////////////////
_adr_size:
or cl,80 ; remember address prefix
jmp _prefix
;;////////////////////////////////////////////////////////////////////
_op_size:
or cl,40 ; remember operand prefix
jmp _prefix
;;////////////////////////////////////////////////////////////////////
_override:
and cl,not 30 ; use closest override
;;////////////////////////////////////////////////////////////////////
_prefix:
inc bp
jmp past_trampoline_prefix
;;////////////////////////////////////////////////////////////////////
_finish:
jmp _finish_
;;////////////////////////////////////////////////////////////////////
_abort:
cmp [cs:condition],offset surface_test
je _finish
xor ax,ax
dec di
sub di,bp
stosw
stosw
dec si
sub si,bp
jmp _finish_abort
;;////////////////////////////////////////////////////////////////////
_aam:
call swap_ints
jmp _two_bytes
;;////////////////////////////////////////////////////////////////////
_push_ds:
and cl,not 10 ; don't set ds=cs
jmp setup_simulate
;;////////////////////////////////////////////////////////////////////
_maybe_5_bytes:
js is_5_bytes
;;////////////////////////////////////////////////////////////////////
_maybe_5_bytes2:
jns _three_bytes
is_5_bytes:
movsb
;;////////////////////////////////////////////////////////////////////

_four_bytes:
movsb
;;////////////////////////////////////////////////////////////////////
_three_bytes:
movsb
;;////////////////////////////////////////////////////////////////////
_two_bytes:
movsb
;;////////////////////////////////////////////////////////////////////
_one_byte:
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

setup_simulate:
cmp [cs:condition],offset surface_test
je past_trampoline6 ; don't execute in a surface engine
pusha
push ds es
pushf
jmp restore_regs
restore_proc_state:
sub sp,16
popf
pop es ds
popa
test cl,6
jz no_int
call swap_ints
jpe trace_int
no_int:
and cl,1
past_trampoline6:
jmp past_trampoline7
trace_int:
inc si
jmp do_int
swap_ints:
push si ax es ds di
pushf
xor di,di
mov es,di
mov si,offset int0ip
stc
intloop1:
mov ax,[es:di]
movs [es:di],[byte ptr cs:si]
mov [si-2],ax
cmc
jnc intloop1
add di,4*4
cmp di,5*4
jb intloop1
xor cl,4
popf
pop di es ax si
ret

;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;////////////////////////////////////////////////////////////////////
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_movseg:
mov bl,10
push ax
rcl al,3
pop ax
jc goto_ModRM
jz goto_ModRM
jpe test_cl_40
js goto_ModRM
or al,bl ; change cs to ds
or cl,bl
jmp goto_ModRM
test_cl_40:
test cl,bl ; is ds set to cs?
jz goto_ModRM
or cl,20 ; set es to previous ds value
xor al,18 ; change es to ds
goto_ModRM:
inc si
stosb
jmp after_save_ModRM
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
escape_opcode:
cmp al,80
jb escape_abort ; privileged opcodes are not traced
cmp al,90
jb escape_jcond
movsb
cmp al,0d0
jae escape_abort
cmp al,0a0
jae use_table
jmp escape_jump_ModRM
use_table:
push di cx
call escape_table_end
escape_table:
db 0a0,0a1,0a2,0a8,0a9
db 0a6,0a7,0ae,0b8,0b9
db 0c2,0c3,0c4,0c5,0c6
db 0a4,0ac,0ba
escape_table_end:
pop di
mov cx,escape_table_end-escape_table
repne scasb
pop cx
jne escape_jump_ModRM_pop
xchg ax,dx
lea ax,[di-offset escape_table-1]
pop di
db 0d4,5
test ah,3
rcr ah,1
xchg ax,dx
jz escape_jump_1byte
jpe escape_jump_Mp1
jc escape_jump_abort
; db ej1 ; 0f a0 - push fs
; db ej1 ; 0f a1 - pop fs
; db ej1 ; 0f a2 - cpuid
; db ejM ; 0f a3 - bt r/m16-32,r16-32
; db ejM1 ; 0f a4 - shld r/m16-32, xx
; db ejM ; 0f a5 - shld r/m16-32, cl
; db eja
; db eja
; db ej1 ; 0f a8 - push gs
; db ej1 ; 0f a9 - pop gs
; db eja ; 0f aa - rsm
; db ejM ; 0f ab - bts r/m16-32
; db ejM1 ; 0f ac - shrd r/m16-32, xx
; db ejM ; 0f ad - shrd r/m16-32, cl
; db eja
; db ejM ; 0f af - imul r/m16-32
; db ejM ; 0f b0 - cmpxhcg r/m8
; db ejM ; 0f b1 - cmpxchg r/m16-32
; db ejM ; 0f b2 - lss r/m16-32
; db ejM ; 0f b3 - btr r/m16-32
; db ejM ; 0f b4 - lfs r/m16-32
; db ejM ; 0f b5 - lgs r/m16-32
; db ejM ; 0f b6 - movzx r/m8
; db ejM ; 0f b7 - movzx r/m16-32
; db eja
; db eja
; db ejM1 ; 0f ba - bt/bts/btr/btc r/m16-32,xx
; db ejM ; 0f bb - btc r/m16-32
; db ejM ; 0f bc - bsf r/m16-32
; db ejM ; 0f bd - bsr r/m16-32
; db ejM ; 0f be - movsx r/m8
; db ejM ; 0f bf - movsx r/m16-32
; db ejM ; 0f c0 - xadd r/m16-32
; db ejM ; 0f c1 - xadd r/m16-32
; db eja,eja,eja,eja,eja
; db ejM ; 0f c7 - cmpxchg8b r/m16-32
; db ej1 ; 0f c8 - bswap eax
; db ej1 ; 0f c9 - bswap ecx
; db ej1 ; 0f ca - bswap edx
; db ej1 ; 0f cb - bswap ebx
; db ej1 ; 0f cc - bswap esp
; db ej1 ; 0f cd - bswap ebp
; db ej1 ; 0f ce - bswap esi
; db ej1 ; 0f cf - bswap edi
;ej1 = 0
;ejM = (escape_jump_ModRM - escape_jump_1byte)
;ejM1 = (escape_jump_Mp1 - escape_jump_1byte)
;eja = (escape_jump_abort - escape_jump_1byte)
escape_jump_ModRM_pop:
pop di
escape_jump_ModRM:
mov ax,[si]
to_ModRM:
jmp do_ModRM
escape_jump_1byte:
jmp setup_simulate
escape_jump_Mp1:
lodsw
stosb
dec si
jmp to_ModRM
escape_jump_abort:
escape_abort:
dec si
dec si
jmp _abort
escape_jcond:
cmp [cs:condition],offset surface_test
je escape_jump_abort
sub al,10 ; change to near jcond
mov [cs:jump_code],al
lodsw
jmp do_cond_near
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
_finish_:
mov [cs:tunnel_ip],si
mov [cs:tunnel_cs],ds
_finish_abort:
cmp [cs:condition],offset surface_test
je finish_surface
mov di,offset simulate_buf
or [byte ptr cs:di+(flags-simulate_buf)+1],ch ; set trap
mov al,0ea
stosb
xchg ax,si
stosw
mov ax,ds
stosw
jmp restore_regs
finish_surface:
call save_sssp
db 0ea
sip dw ?
scs dw ?
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_cond_jump:
dec si
cmp [cs:condition],offset surface_test
je _finish_
push ax
lodsw
mov [cs:jump_code],al
pop ax
cbw ; ax = thing to adjust ip by
do_cond_near:
mov dx,cx
xor [byte ptr cs:flags+1],ch
push [cs:flags]
mov cx,[cs:_cx] ; in case it is a loop
popf
past_prefix:
jump_code db 70,true-false
false:
mov [cs:_cx],cx
mov cx,dx
past_trampoline7:
jmp past_trampoline8
true:
add si,ax
jmp false ; add instruction size to si
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_ModRM:
movsb
after_save_ModRM:
test al,0c0
pushf
and ax,707
popf
push cx
rcl cl,1
pop cx
jc ModRM32
jz maybe16disp
jpe nodisp
jns disp8
jmp disp16
ModRM32:
jz maybe32disp
jpe nodisp
jns disp8_SIB
mov ah,5
jmp maybe32disp
maybe16disp:
cmp al,6
je disp16
jmp nodisp
disp8_SIB:
movsb
mov ah,al
cmp al,4
jne disp8
maybe32disp:
cmp al,5
je disp32
cmp al,4
jne nodisp
movsb
cmp ah,5
jne nodisp
disp32:
movsw
disp16:
movsb
disp8:
movsb
nodisp:
jmp setup_simulate
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_int_ret:
call save_sssp
pop si ds ax
call restore_sssp
mov [cs:flags],ax
and ax,100
xor [cs:flags],ax ; turn off trap flag
or cx,ax ; remember trap flag state
past_trampoline8:
jmp past_trampoline9
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_int:
mov ah,0
call save_sssp
mov bx,[cs:flags] ; set trap flag to current value
or bh,ch
push bx ds si ; save just like real int
call restore_sssp
and [byte ptr cs:flags+1],not 3 ; mask IF and TF like real int
xor cx,cx
mov ds,cx
shl ax,2 ; ax set to int number by caller (us)
xchg ax,si ; ds:si = int vector
lds si,[si] ; ds:si = new cs :)
past_trampoline9:
jmp past_trampoline ; start over n stuph
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
do_ret_iw:
call save_sssp
pop si
add sp,ax
call restore_sssp
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;////////////////////////////////////////////////////////////////////
ModRM_plus_2 = (_ModRM_plus_2 - past_table)
ModRM_plus_2_f7 = (_ModRM_plus_2_f7 - past_table)
ModRM_plus_1 = (_ModRM_plus_1 - past_table)
ModRM_plus_1_f6 = (_ModRM_plus_1_f6 - past_table)
ModRM_ff = (_ModRM_ff - past_table)
bound_ = (_bound - past_table)
ModRM = (_ModRM - past_table)
push_ds = (_push_ds - past_table)
push_cs = (_push_cs - past_table)
do_pushf = (_pushf - past_table)
do_popf = (_popf - past_table)
escape = (_escape - past_table)
cs_override = (_cs_override - past_table)
prefix = (_prefix - past_table)
maybe_5_bytes = (_maybe_5_bytes - past_table)
maybe_5_bytes2 = (_maybe_5_bytes2 - past_table)
four_bytes = (_four_bytes - past_table)
three_bytes = (_three_bytes - past_table)
aam_ = (_aam - past_table)
two_bytes = (_two_bytes - past_table)
one_byte = (_one_byte - past_table)
loop_ne = (_loop_ne - past_table)
loop_e = (_loop_e - past_table)
do_loop = (_do_loop - past_table)
cond_jump = (_cond_jump - past_table)
return = (_return - past_table)
ret_iw = (_ret_iw - past_table)
returnf = (_returnf - past_table)
retf_iw = (_retf_iw - past_table)
int3 = (_int3 - past_table)
interrupt = (_interrupt - past_table)
int_ov = (_int_ov - past_table)
bpice = (_bpice - past_table)
int_ret = (_int_ret - past_table)
jmp_short = (_jmp_short - past_table)
jmp_near = (_jmp_near - past_table)
jmp_far = (_jmp_far - past_table)
far_call = (_far_call - past_table)
do_call = (_do_call - past_table)
adr_size = (_adr_size - past_table)
op_size = (_op_size - past_table)
override = (_override - past_table)
prefix = (_prefix - past_table)
mov_seg = (_mov_seg - past_table)
finish = (_finish - past_table)
abort = (_abort - past_table)

;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

past_trampoline:
cli
push cs
pop es
call [word ptr cs:condition]
jnc go_deeper
jmp _finish_
go_deeper:
xor bp,bp
past_trampoline_prefix:
test_trap_flag:
mov ax,101
test ch,al
jz goto_zrampoline
xor cl,al
xor al,cl
jz goto_zrampoline ; IF enables int1 after 1 op
or [byte ptr cs:flags+1],al ; set trap flag
sub cx,ax
cbw
jmp do_int
goto_zrampoline:
cld
lea di,[bp+offset simulate_buf]
;;=========//////////: set ES:DI to the location of the code buffer plus any
;;=========//////////: prefixes (BP) and initialize the buffer to nops
mov ax,190
push cx
mov cx,10
sub cx,bp
push cx
rep stosb ; fill simulate_buf with nops
pop cx
sub di,cx ; es:di = cs:simulate_buf
pop cx
lodsb
stosb
mov dx,[si]
mov bx,offset jump_table
segcs xlat
;;=========//////////: set DS:BX = CS:jump_table and then read in the byte
;;=========//////////: offset to jump from based on the first byte of the
;;=========//////////: next instruction (which is contained in AL)
add bx,ax ; bx = offset to jump to
xchg ax,dx
test cl,0c0
jmp bx
int5:
mov [byte ptr cs:offset tos-17],5 ; set al to 5
rest_o_int:
add sp,3
jmp after_simulate
int0:
mov [byte ptr cs:offset tos-17],0 ; set al to 0
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
calc_ModRMff:
push ds ax
lodsb
test al,8
push ax
mov al,0c5 ; use LDS AX,r/m16-32 for far jump/call
jnz used_lds
use_mov:
xor al,8bh xor 0c5 ; use MOV AX,r/m16-32 for near jump/call
used_lds:
dec di
stosb
pop ax
and al,0c7
stosb
or cl,8
jmp after_save_ModRM
after_calc:
sub sp,3e
mov es,ax
popa
pop bx
mov ax,ds
cmp ax,[cs:_ds]
mov ax,es
jne ds_ok
pop ds
push ds
ds_ok:
pop es
call save_sssp
xor bp,bp
shl bl,2
test bl,0e0
js ff_jumps
jpo ff_call_near
push es
ff_call_near:
push si
ff_jumps:
xchg ax,si
call restore_sssp
and cl,1
jmp past_trampoline
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
save_sssp:
pop [word ptr cs:_stack_]
mov dx,sp
mov ss,[cs:_ss]
mov sp,[cs:_sp]
continue_sssp:
jmp [word ptr cs:_stack_]
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
restore_sssp:
pop [word ptr cs:_stack_]
mov [cs:_sp],sp
mov sp,cs
mov ss,sp
mov sp,dx
jmp continue_sssp
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

after_simulate:
save_regs:
mov [cs:_ss],ss
mov [cs:_sp],sp
mov sp,cs
mov ss,sp
mov sp,offset regs
pushf
test [cs:_cl],83
js after_calc
pusha
jz save_both
jpe save_nuthin
mov cx,ds
cmp cx,[cs:_cs]
je save_es
save_both:
push ds
cmp ax,0
org $-2
save_es:
dec sp
dec sp
push es
past_es:
save_nuthin:
mov sp,cs
mov ss,sp
mov sp,offset tos-16
db 0e9
save_ret_to:
dw restore_proc_state-after_save
after_save:

restore_regs:
rol cl,4
mov [cs:_cl],cl
mov [cs:_cs],ds
mov sp,offset bor
test cl,3 ; set DS=CS, or ES=old DS?
mov bx,ds ; bx=cs
pop ds es
mov ax,ds
jz past_stuff_ds_es
mov ds,bx
jpo past_stuff_ds_es
mov es,ax
past_stuff_ds_es:
popa
popf
pop ss
mov sp,[cs:_sp]
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simulate_buf:
db 10 dup (90)
jmp after_simulate
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tunnel_test:
cmp [byte ptr si],0cf
je not_there
mov ax,si
shr ax,4 ; un-segmentize
;; segmentize our current location
mov bx,ds
add ax,bx
cmp ax,[word ptr cs:firstMCB]
not_there:
ret
;;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
surface_test:
cmp [cs:num_instrucs],20
pushf
inc [cs:num_instrucs]
popf
ret

int0ip dw ?
int0cs dw ?
int5ip dw ?
int5cs dw ?
_stack_ dw 30 dup (?)
tos: ; top of stack
bor: ; bottom of registers
_ds dw ?
_es dw ?
_di dw ?
_si dw ?
_bp dw ?
_tr dw ?
_bx dw ?
_dx dw ?
_cx dw ?
_ax dw ?
flags dw ?
regs:
_ss dw ?
_sp dw ?
_cl db 0
_cs dw ?
condition dw offset tunnel_test
num_instrucs db 0
firstMCB dw ?
tunnel_ip dw ?
tunnel_cs dw ?
ends code
end start
