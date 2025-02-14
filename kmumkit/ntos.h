#pragma once
#pragma region all
#include "amd64.h"
#include "conf.h"
#include "dbg.h"
#include "sig.h"
#pragma endregion

#ifdef __km__
#include <ntddk.h>
#endif // __km__

#ifdef __um__
#include <windows.h>
#include "ntdll.h"
#include "win.h"

void*	  ntGetCiOptions ();
physaddr  ntGetPml4Base (funcReadPa fnRead);
uintptr_t ntGetRvaCiOptions (IN void* pImageBaseKrnl);
void*	  ntGetImageBase (IN const wchar_t* szModuleName);
#endif // __um__