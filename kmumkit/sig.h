#pragma once
#pragma region all
void* scanFreePit (IN void* pBase, IN const char* szSectionName, IN size_t size);
void* scan (IN void* pBase, IN const char* pPattern, IN size_t sizePattern, IN size_t sizeScan);
void* scanInSection (IN void* pBase, IN const char* szSectionName, IN const char* pPattern, IN size_t sizePattern);
#pragma endregion
