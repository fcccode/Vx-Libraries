.586
.model flat, stdcall
locals
jumps

DDS_STRUCT_SIZE             equ    1ch 


include              win32api.inc
include              useful.inc

extrn         VirtualAlloc:proc
extrn         ExitProcess:proc
extrn         LoadLibraryA:proc
extrn         GetProcAddress:proc

.data
                     dde           dd     ?
                     assembly      dd     ?
                     api           db     "VirtualAllocEx",0
                     dll           db     "kernel32.dll",0

.code
__start:
                     call   VirtualAlloc, 0, 1000h, MEM_COMMIT, PAGE_READWRITE
                     mov    dde, eax
                     call   VirtualAlloc, 0, 1000h, MEM_COMMIT, PAGE_READWRITE
                     mov    assembly, eax
                                        
                     push   offset api
                     push   offset dll
                     call   LoadLibraryA
                     push   eax
                     call   GetProcAddress
                     mov    ebx, eax
                     mov    edx, dde
                     mov    ecx, ebx
                     add    ecx, 50h
;;;;
                     mov    ebx, 401000h
                     mov    ecx, offset __done

__cycle:             call   dde_disassm, ebx, edx
                     add    ebx, eax               
                     add    edx, DDS_STRUCT_SIZE
                     cmp    ebx, ecx
                     jb     __cycle
            
                     mov    ebx, assembly
                     mov    edx, dde         

__cycle_asm:         cmp    dword ptr[edx], 0
                     je     __done
                     call   dde_asm, ebx, edx
                     add    ebx, eax
                     add    edx, DDS_STRUCT_SIZE
                     jmp    __cycle_asm                                   


__done:              call   ExitProcess, 0        




include              dde32bin.inc

end    __start