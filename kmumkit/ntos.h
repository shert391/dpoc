#pragma once
#pragma region all
#include "amd64.h"
#include "dbg.h"
#pragma endregion

#ifdef __km__
#include <ntddk.h>
#endif // __km__

#ifdef __um__
#include <windows.h>
#include "ntdll.h"

void*	  ntGetCiOptions ();
uintptr_t ntGetRvaCiOptions ();
physaddr  ntGetPml4Base (funcReadPa fnRead);
void*	  ntGetImageBase (IN const wchar_t* szModuleName);
#endif // __um__