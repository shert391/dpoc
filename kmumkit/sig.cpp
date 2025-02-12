#include "sig.h"

#pragma region all
void* scanInSection(_In_ void* pImageBase, _In_ const char* pSectionName, _In_ const char* pPattern, _In_ size_t sizePattern)
{
	auto* pDosHeader = (IMAGE_DOS_HEADER*)pImageBase;
	auto* pNtHeaders = (IMAGE_NT_HEADERS64*)((uintptr_t)pImageBase + pDosHeader->e_lfanew);

	for (IMAGE_SECTION_HEADER* curSection = IMAGE_FIRST_SECTION(pNtHeaders); curSection != nullptr; curSection++)
	{
		if (strcmp((PCSTR)curSection->Name, pSectionName) == 0) {
			auto* pBaseSection = (void*)((uintptr_t)pImageBase + curSection->VirtualAddress);
			return scan(pBaseSection, pPattern, sizePattern, curSection->SizeOfRawData);
		}
	}

	return nullptr;
}

void* scan(_In_ void* pBase, _In_ const char* pPattern, _In_ size_t sizePattern, _In_ size_t sizeScan)
{
	for (size_t i = 0; i < sizeScan; i++)
	{
		size_t j{ 0 };
		for (; j < sizePattern; j++)
			if (pPattern[j] != *(char*)((uintptr_t)pBase + i + j) && pPattern[j] != '\x00') // Опасный сканнер: в x86 есть кодирование с 0x00 - нужно исправить, например, наложить маску
				break;
		if (j == sizePattern)
			return (void*)((uintptr_t)pBase + i);
	}

	return nullptr;
}
#pragma endregion