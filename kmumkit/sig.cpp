#include "global.h"

#pragma region all

void* scanInSection (IN void* pBase, IN const char* szSectionName, IN const char* pPattern, IN size_t sizePattern) {
    auto* pDosHeader = (IMAGE_DOS_HEADER*)pBase;
    auto* pNtHeaders = (IMAGE_NT_HEADERS64*)((uintptr_t)pBase + pDosHeader->e_lfanew);
    for (IMAGE_SECTION_HEADER* curSection = IMAGE_FIRST_SECTION(pNtHeaders); curSection != nullptr; curSection++) {
        if (strcmp((PCSTR)curSection->Name, szSectionName) == 0) {
            auto* pBaseSection = (void*)((uintptr_t)pBase + curSection->VirtualAddress);
            return scan(pBaseSection, pPattern, sizePattern, curSection->SizeOfRawData);
        }
    }
    return nullptr;
}

void* scan (IN void* pBase, IN const char* pPattern, IN size_t sizePattern, IN size_t sizeScan) {
    for (size_t i = 0; i < sizeScan; i++) {
        size_t j {0};
        for (; j < sizePattern; j++)
            if (pPattern[j] != *(char*)((uintptr_t)pBase + i + j) && pPattern[j] != '\x00') // Опасный сканнер: в x86 есть кодирование с 0x00 - нужно исправить, например, наложить маску
                break;
        if (j == sizePattern)
            return (void*)((uintptr_t)pBase + i);
    }
    return nullptr;
}

void* scanRepeatInv (IN void* pBase, IN char number, IN __int64 sizeRepeat, IN __int64 sizeScan) {
    for (__int64 i = 0; i > -sizeScan; i--) {
        __int64 j {0};
        for (j = 0; j > (-sizeRepeat); j--)
            if (number != *(char*)((uintptr_t)pBase + i + j))
                break;
        if (j == (-sizeRepeat))
            return (void*)((uintptr_t)pBase + i + j + 1);
    }

    return nullptr;
}

void* scanFreePit (IN void* pBase, IN const char* szSectionName, IN size_t size) {
    auto* pDosHeader = (IMAGE_DOS_HEADER*)pBase;
    auto* pNtHeaders = (IMAGE_NT_HEADERS64*)((uintptr_t)pBase + pDosHeader->e_lfanew);
    for (IMAGE_SECTION_HEADER* curSection = IMAGE_FIRST_SECTION(pNtHeaders); curSection != nullptr; curSection++) {
        if (strcmp((PCSTR)curSection->Name, szSectionName) == 0) {
            auto* pBaseSection = (void*)((uintptr_t)pBase + curSection->VirtualAddress);
            auto* pPit         = (void*)((uintptr_t)pBaseSection + curSection->SizeOfRawData - 1);
            return scanRepeatInv(pPit, 0x00, size, PAGE_4K_SIZE);
        }
    }
    return nullptr;
}

#pragma endregion