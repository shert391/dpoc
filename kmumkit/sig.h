#pragma once

#ifdef __km__
#include <ntddk.h>
#endif // __km__

#ifdef __um__
#include <windows.h>
#endif __um__

#pragma endregion
void* scan(_In_ void* pBase, _In_ const char* pPattern, _In_ size_t sizePattern, _In_ size_t sizeScan);
void* scanInSection(_In_ void* pImageBase, _In_ const char* pSectionName, _In_ const char* pPattern, _In_ size_t sizePattern);
#pragma region all
