﻿#include "global.h"

void* fFileMap (IN const wchar_t* szPath) {
#ifdef __um__
    OFSTRUCT fInfo {0};
    HANDLE   hFile = CreateFile(szPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    chandle(hFile);

    LARGE_INTEGER exInt {0};
    GetFileSizeEx(hFile, &exInt);

    void* pMapPages = VirtualAlloc(nullptr, exInt.QuadPart, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    cnull(pMapPages);

    ctrue(ReadFile(hFile, pMapPages, exInt.QuadPart, nullptr, nullptr));

    CloseHandle(hFile);

    dbg("pRawImage = 0x%p; szPath = %ls; rawSize = %lli bytes; File mapped success!", pMapPages, szPath, exInt.QuadPart);
    return pMapPages;
#endif // __um__

    return nullptr;
}

void fMapSections (IN void* pFile, OUT void* pImage) {
    PIMAGE_NT_HEADERS64   pNtHeaders     = IMAGE_NT_HEADERS64(pFile);
    PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);

    for (size_t i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++) {
        if (!pSectionHeader->SizeOfRawData) continue;
        void*  sectionVA   = (void*)((uintptr_t)pImage + pSectionHeader->VirtualAddress);
        void*  pRawSection = (void*)((uintptr_t)pFile + pSectionHeader->PointerToRawData);
        size_t rawSize     = pSectionHeader->SizeOfRawData;
        memcpy(sectionVA, pRawSection, rawSize);
        dbg("Section %s mapped! pRawSection = 0x%p; rawSize = %lli bytes; VA = 0x%p", pSectionHeader->Name, pRawSection, rawSize, sectionVA);
    }
}

void fFixImports (IN void* pFile, OUT void* pImage) {
    PIMAGE_NT_HEADERS64 pNtHeaders     = IMAGE_NT_HEADERS64(pFile);
    uintptr_t           importTableRVA = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    void*               importTableVA  = (void*)((uintptr_t)pImage + importTableRVA);
    dbg("Import Table RVA = 0x%llX; Import Table VA = 0x%p", importTableRVA, importTableVA);

    PIMAGE_IMPORT_DESCRIPTOR pDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)importTableVA;
    for (; *(uintptr_t*)pDescriptor; pDescriptor++) {
        char*   szNameLib = (char*)((uintptr_t)pImage + pDescriptor->Name);
        HMODULE hLib      = LoadLibraryA(szNameLib);
        chandle(hLib);

        void* pIAT = (void*)((uintptr_t)pImage + pDescriptor->FirstThunk);
        void* pOTs = (void*)((uintptr_t)pImage + pDescriptor->OriginalFirstThunk);
        dbg("Start fix import for %s; pIAT = 0x%p; pOTs = 0x%p", szNameLib, pIAT, pOTs);

        PIMAGE_THUNK_DATA64 pThunk     = (PIMAGE_THUNK_DATA64)pOTs;
        void**              ppFunction = (void**)pIAT;

        for (; *(uintptr_t*)pThunk; pThunk++, ppFunction++) {
            if (IMAGE_SNAP_BY_ORDINAL(*(uintptr_t*)pThunk)) {
                void* pFunc = GetProcAddress(hLib, (char*)IMAGE_ORDINAL(pThunk->u1.Ordinal));
                cnull(pFunc);
                *ppFunction = pFunc;
                dbg("Fix import by ordinal %llX", IMAGE_ORDINAL(pThunk->u1.Ordinal));
                continue;
            }

            PIMAGE_IMPORT_BY_NAME pInfoImportByName = (PIMAGE_IMPORT_BY_NAME)((uintptr_t)pImage + pThunk->u1.AddressOfData);
            void*                 pFunc             = GetProcAddress(hLib, pInfoImportByName->Name);
            cnull(pFunc);
            *ppFunction = pFunc;
            dbg("Fix import by name %s", pInfoImportByName->Name);
        }
    }
}

void fFixReloc (IN void* pFile, OUT void* pImage) {
    PIMAGE_NT_HEADERS64 pNtHeaders    = IMAGE_NT_HEADERS64(pFile);
    uintptr_t           relocTableRVA = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    void*               relocTableVA  = (void*)((uintptr_t)pImage + relocTableRVA);

    PIMAGE_BASE_RELOCATION pHeaderRelocBlock = (PIMAGE_BASE_RELOCATION)relocTableVA;
    for (; *(uintptr_t*)pHeaderRelocBlock; pHeaderRelocBlock = (PIMAGE_BASE_RELOCATION)(NEXT_RELOC_BLOCK(pHeaderRelocBlock))) {
        PWORD pEntry     = (PWORD)(pHeaderRelocBlock + 1);
        WORD  countEntry = COUNT_ENTRY_IN_RELOC_BLOCK(pHeaderRelocBlock);
        for (size_t i = 0; i < countEntry; i++, pEntry++) {
            if (TYPE_RELOC(*pEntry) != IMAGE_REL_BASED_DIR64)
                continue;

            __int64  delta = (__int64)pImage - pNtHeaders->OptionalHeader.ImageBase;
            __int64* pFix  = (__int64*)((uintptr_t)pImage + pHeaderRelocBlock->VirtualAddress + ((*pEntry) & 0xFFF));
            dbg("Fix 0x%p for delta 0x%llX", pFix, delta);
            (*pFix) += delta;
        }
    }
}

void fInstallTableDynamicFunctions (IN void* pFile, OUT void* pImage) {
    PIMAGE_NT_HEADERS64 pNtHeaders        = IMAGE_NT_HEADERS64(pFile);
    uintptr_t           exceptionTableRVA = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress;

    if (!exceptionTableRVA) {
        dbg("Exception is not exists");
        return;
    }

    size_t exceptionTableSize = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size;
    void*  exceptionTableVA   = (void*)((uintptr_t)pImage + exceptionTableRVA);

    PRUNTIME_FUNCTION pFirstRuntimeFunction = (PRUNTIME_FUNCTION)exceptionTableVA;
    RtlAddFunctionTable(pFirstRuntimeFunction, exceptionTableSize / sizeof(_IMAGE_RUNTIME_FUNCTION_ENTRY), (DWORD64)pImage);
}

void* fExecuteMap (IN const wchar_t* szPath) {
    void* pFile = fFileMap(szPath);

    size_t sizeImage = (IMAGE_NT_HEADERS64(pFile))->OptionalHeader.SizeOfImage;
    void*  pImage    = VirtualAlloc(nullptr, sizeImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    cnull(pImage);

    fMapSections(pFile, pImage);
    fFixImports(pFile, pImage);
    fFixReloc(pFile, pImage);
    fInstallTableDynamicFunctions(pFile, pImage);

    void* epVA = (void*)((uintptr_t)pImage + (IMAGE_NT_HEADERS64(pFile))->OptionalHeader.AddressOfEntryPoint);
    czero(VirtualFree(pFile, 0, MEM_RELEASE));
    return epVA;
}