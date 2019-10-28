   ;note! this version can have small bugz, i haven't tested if for 100%
   ;only for 99%, if you will find some bugz plz send me a email or
   ;rebuild it yourself ;] i'm a lazy man 
   ;oohh and the argument stripper isn't working correctly - why?
   ;when u debug it w0rx perfectly! eeeh fuck it!
   ;the source is weird and heavy UNOPTIMISED! do it yourself
   ;if you want
   ;
   ;(c) YuP - Deithwen Addan                                    22.01.02
    

   .386p
   .model flat

   callx macro x
   extrn x:proc
   call x
   endm




   .data
   db "Gone with the wind ;]"

   .code
   pack:

   call delta
   delta: pop ebp
   sub ebp,offset delta


   lea eax,[ebp+od]
   push 0
   push eax
   callx _lcreat

   push eax
   callx _lclose

   ;call @parse_cline   ;why this w0rx only on debug!?
   mov ebx,esi
   lea ebx,[ebp+do]     ;this time file to pack is "pack.me"

   call @open_and_map

   lea ebx,[ebp+od]
   call @open_and_map


   ;########################################
   ;****************************************
   ;REAL PACKER ENGINE
   ;****************************************
   ;pMapReal dd 0
   ;fMapReal dd 0
   ;fSize    dd 0


   mov ecx,dword ptr [ebp+fSize]
   mov edi,dword ptr [ebp+pMapReal]
   mov esi,dword ptr [ebp+fMapReal]
   mov edx,esi

   mov eax,ecx
   inc eax
   mov dword ptr [ebp+bytez],eax


   one_more_time:

   dalej:
   xor eax,eax
   xor ebx,ebx
   lodsb
   inc edx



   compare:
   dec dword ptr [ebp+bytez]
   cmp dword ptr [ebp+bytez],1
   jle packed

   cmp byte ptr [edx],al
   jne @there_is_no_repeat_left

   inc edx
   inc ebx
   inc esi

   
   for_l: loop compare
          jmp packed


   @there_is_no_repeat_left:
   test ebx,ebx
   jnz @take_a_shot

   pushad
   mov edx,dword ptr [ebp+new_size]
   mov ecx,dword ptr [ebp+bytez]
   mov byte ptr [edi],al
   popad


   inc edi
   inc dword ptr [ebp+new_size]
   jmp leavee

   leavee: jmp one_more_time

   @take_a_shot:
   cmp ebx,4
   jae @pack_it

   push ecx
   mov ecx,ebx
   inc ecx

   write_old_bytez: mov byte ptr [edi],al
                    inc edi
                    inc dword ptr [ebp+new_size]
                    loop write_old_bytez

   pop ecx
   jmp leavee


   @pack_it:
   mov byte ptr [edi],'¥'
   inc edi
   mov byte ptr [edi],al
   inc edi

   ;at this point we have sth like this:
   ;"¥<BYTE>" now we need to add repeat number
   ;and i found some good solution for this
   ;you know that highest byte is 255 (ascii table)
   ;so i would use ascii characters , no numbers like
   ;1000 etc.

   ;sample if i have a file with sample byte 
   ;which i repeating 256 times
   ;i will save it as: "¥<BYTE>ÿ¥"
   ;value of 1st characters is: 255 and second value is 1                                           ;do you catch it!?  ÿ

   add dword ptr [ebp+new_size],2  ;ADD TWO BYTEZ

   cmp ebx,255      ;highest then highest ;)
   ja  @can_sub


   push eax
   mov eax,ebx
   mov byte ptr [edi],al
   pop eax
   inc edi

   jmp next


   @can_sub:
   push eax
   sub_it:
   sub ebx,255
   mov byte ptr [edi],255
   inc edi
   inc dword ptr [ebp+new_size]
   cmp ebx,255
   ja sub_it

   mov eax,ebx
   mov byte ptr [edi],al
   inc edi
   pop eax

   next:
   mov byte ptr [edi],'¥'
   inc edi
   add dword ptr [ebp+new_size],2
   jmp leavee

   packed: mov ecx,dword ptr [ebp+new_size]
           call _out3

   dupa:
   lea ebx,[ebp+od]
   xor eax,eax
   push eax
   push 00000080h
   push 00000003h
   push eax
   push eax
   push 80000000h OR 40000000h
   push ebx
   callx CreateFileA
   mov dword ptr [ebp+fHnd],eax


   call TruncFile


   push dword ptr [ebp+fHnd]
   callx CloseHandle

        
   exit: push dword ptr [ebp+new_size]
         callx ExitProcess


   @fuckin_error: call _out3
                  lea esi,[ebp+e5]
                  call error
                  jmp exit
   

   is_end: dec dword ptr [ebp+bytez]
           jz packed
           ret
   
   ;##############################################
   ;##############################################





   ;****************************
   ;OUT: EBX - FILE TO PACK
   ;****************************
   @parse_cline:
   callx GetCommandLineA
   mov esi,eax
   xor ebx,ebx

   _loop: inc ebx
          lodsb
          cmp al,' '
          jne _loop


   xor ebx,ebx
   push esi
   lodsb
   cmp al,0
   pop esi
   mov ebx,esi
   jne o
   lea esi,[ebp+e0]
   call error
   jmp exit
   mov ebx,esi
   o: ret

   ;****************************   
   ;ENTRY ESI: BUFFER
   ;****************************
   error:
   pushad
   lea eax,[ebp+packer]
   push 00000010h
   push eax
   push esi
   push 0
   callx MessageBoxA
   popad
   ret

     ;-----------------------------------------------------------------------------------------------
;MAPING PROCEDURES ! SHIT ;P
;-----------------------------------------------------------------------------------------------
;'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
   ;********************
   ;ECX - SIZE TO MAP
   ;********************
   @open_and_map:
   lea esi,[ebp+offset e2]
   xor eax,eax
   push eax
   push 00000080h
   push 00000003h
   push eax
   push eax
   push 80000000h OR 40000000h
   push ebx
   callx CreateFileA
   mov edx,dword ptr [ebp+fHnd]
   test edx,edx
   jnz pack_f1


   mov dword ptr [ebp+fHnd],eax

   pack_f1:
   mov dword ptr [ebp+pHnd],eax
   mov edx,eax

   inc eax
   jz was_wrong


   push edx
   ;------------THIS API FUCKZ EDX!!!---------------
   push 0
   push dword ptr [ebp+fHnd]
   callx GetFileSize
   mov dword ptr [ebp+fSize],eax


   mov ecx,eax
   pop edx



   @vmap:
   xor eax,eax
   push eax
   push dword ptr [ebp+fSize]
   push eax
   push 00000004h
   push eax
   push edx
   callx CreateFileMappingA
   test eax,eax
   mov edi,eax
   jz _out2


   mov edx,dword ptr [ebp+fHndMap]
   test edx,edx
   jnz pack_f2

   mov dword ptr [ebp+fHndMap],eax
   push dword ptr [ebp+fSize]
   mov dword ptr [ebp+fSize],0

   pack_f2: mov dword ptr [ebp+pHndMap],eax


   @vview:
   xor eax,eax
   push dword ptr [ebp+fSize]
   push eax
   push eax
   push 00000004h OR 00000002h
   push edi
   callx MapViewOfFile
   test eax,eax
   mov edi,eax
   jz _out3

   mov edx,dword ptr [ebp+fMapReal]
   test edx,edx
   jnz pack_f3

   mov dword ptr [ebp+fMapReal],eax
   pop dword ptr [ebp+fSize]

   pack_f3: mov dword ptr [ebp+pMapReal],eax

   goo:ret

   _out3: 
          push dword ptr [ebp+fMapReal]
          callx UnmapViewOfFile
          mov edx,dword ptr [ebp+pMapReal]
          test edx,edx
          jz upack_f1       
          push dword ptr [ebp+pMapReal]
          callx UnmapViewOfFile
          jmp _out2

          upack_f1:
          test edi,edi
          jnz _out2
          lea esi,[ebp+e4]
          call error
       
 


   _out2: push dword ptr [ebp+fHndMap]
          callx CloseHandle
          mov edx,dword ptr [ebp+pHndMap]
          test edx,edx
          jz upack_f2       
          push dword ptr [ebp+pHndMap]
          callx CloseHandle
          jmp _out

          upack_f2:
          test edi,edi
          jnz _out
          lea esi,[ebp+e3]
          call error
       
       

   _out:  push dword ptr [ebp+fHnd]
          callx CloseHandle
          test edi,edi
          jz go       
          mov edx,dword ptr [ebp+pHnd]      
          lea esi,[ebp+yep]
          call error
    
   go:    push dword ptr [ebp+pHnd]
          callx CloseHandle
          jmp dupa
       

             

   was_wrong:
   call error
   jmp exit

   TruncFile:
   xor     eax,eax
   push    eax
   push    eax
   push    dword ptr [ebp+new_size]
   push    dword ptr [ebp+pHnd]
   callx   SetFilePointer

   push    dword ptr [ebp+pHnd]
   callx   SetEndOfFile
   ret


;************************************************************************************************
;------------------------------------------------------------------------------------------------
;*****************************************************DATA***************************************
   packer db "[YSPE] - (c) YuP :/: Deithwen Addan",0 

   do db "pack.me",0            ;change it ;]
   od db "packed.yspe",0

   e0 db "USAGE: PACKER.EXE <FILE_TO_PACK>",0
   e2 db "Can't open file!",0
   e3 db "Can't map file!",0
   e4 db "Can't make real view!",0
   e5 db "System fuckOUT!",0
 

   yep db "FILE PACKED!!!",0

   new_size dd 0
   bytez    dd 0

   pHnd     dd 0
   pMapReal dd 0
   pHndMap  dd 0


   fHnd     dd 0
   fMapReal dd 0
   fHndMap  dd 0
   fSize    dd 0

   end pack


