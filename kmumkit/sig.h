#pragma once

#ifdef __km__
#include <ntddk.h>
#endif // __km__

#ifdef __um__
#include <windows.h>
#endif __um__

#pragma endregion
void* scan (IN void* pBase, const IN char* pPattern, IN size_t sizePattern, IN size_t sizeScan);
void* scanInSection (IN void* pImageBase, const IN char* pSectionName, const IN char* pPattern, IN size_t sizePattern);
#pragma region all
