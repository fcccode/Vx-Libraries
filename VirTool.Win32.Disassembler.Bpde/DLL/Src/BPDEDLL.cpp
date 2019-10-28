// BPDEDLL.cpp : Defines the entry point for the DLL application.
//
#include <windows.h>
#include "BPDEDLL.h"
#include "BPDE.h"//include BPDE Disassmebler Engine

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// This is an example of an exported function.
BPDEDLL_API int Disasm(char *Offset,char *Instruction)
{
	disasm(Offset,(_Code*)Instruction);
	return 0;
}