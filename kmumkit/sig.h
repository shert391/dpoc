#pragma once
#pragma region all
void* scan (IN void* pBase, const IN char* pPattern, IN size_t sizePattern, IN size_t sizeScan);
void* scanInSection (IN void* pImageBase, const IN char* pSectionName, const IN char* pPattern, IN size_t sizePattern);
#pragma endregion
