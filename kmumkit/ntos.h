#pragma once
#ifdef __um__
void*	  ntGetCiOptions ();
physaddr  ntGetPml4Base (funcReadPa fnRead);
uintptr_t ntGetRvaCiOptions (IN void* pImageBaseKrnl);
void*	  ntGetImageBase (IN const char* szModuleName);
#endif // __um__