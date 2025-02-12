#pragma once
#pragma region all
#include "dbg.h"
#include "amd64.h"
#pragma endregion

#ifdef __km__
#include <ntddk.h>
#endif // __km__

#ifdef __um__
#include "ntdll.h"
#include <windows.h>

void* ntGetCiOptions();
uintptr_t ntGetRvaCiOptions();
physaddr ntGetPml4Base(funcReadPa fnRead);
void* ntGetImageBase(_In_ const wchar_t* szModuleName);
#endif // __um__