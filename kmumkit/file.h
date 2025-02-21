#pragma once

#define IMAGE_NT_HEADERS64(p) (IMAGE_NT_HEADERS64*)((uintptr_t)p + ((IMAGE_DOS_HEADER*)p)->e_lfanew)

void* fFileMap (IN const wchar_t* szPath);
void* fExecuteMap (IN const wchar_t* szPath);