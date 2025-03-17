#include "global.h"

#define ENABLE_AMD64_SHADOW_STACK

#pragma region all

void ntUnicodeStringToWchar (IN PUNICODE_STRING pUnicodeString, OUT wchar_t* pOut) {
    memcpy(pOut, pUnicodeString->Buffer, pUnicodeString->Length);
}

#pragma endregion

#ifdef __um__
void* ntGetCiOptions () {
    void* pCiImageBase = ntGetImageBase("CI.dll");
    auto  rvaCiOptions = ntGetRvaCiOptions(pCiImageBase);
    void* pCiOptions   = (void*)((uintptr_t)pCiImageBase + rvaCiOptions);
    return pCiOptions;
}

// брутфорс 2^20 = 1MB памяти реального режима в поисках _PROCESSOR_START_BLOCK
// на самом деле структура обычно валяется после 10-го PFN
// fnRead любой примитив чтения физ.памяти. Например, функция уязвимого провайдера
physaddr ntGetPml4Base (funcReadPa fnRead) {
    _PROCESSOR_START_BLOCK psb {0};

    for (uintptr_t pa = 0x10000; pa <= RM_SIZE; pa += PAGE_4K_SIZE) {
        fnRead((physaddr)pa, (void*)&psb, sizeof(psb));
        if (psb.Jmp.OpCode != 0xe9 || psb.CompletionFlag != true) continue;
        return (physaddr)psb.ProcessorState.SpecialRegisters.Cr3;
    }

    return nullptr;
}

void* ntGetImageBase (IN const char* szModuleName) {
    void* ret = nullptr;

    ULONG    size {0};
    NTSTATUS ntstatus = NtQuerySystemInformation(SystemModuleInformation, nullptr, 0, &size);

    PRTL_PROCESS_MODULES pProcessModules = (PRTL_PROCESS_MODULES)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    ntstatus                             = NtQuerySystemInformation(SystemModuleInformation, pProcessModules, size, nullptr);
    cntstatus(ntstatus);

    PRTL_PROCESS_MODULE_INFORMATION pModuleInfo = &pProcessModules->Modules[0];

    for (size_t i = 0; i < pProcessModules->NumberOfModules; i++, pModuleInfo++) {
        PCHAR szCurrModuleName = (PCHAR)pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;

        if (memcmp(szCurrModuleName, szModuleName, sizeof(szModuleName)) != 0)
            continue;

        ret = pModuleInfo->ImageBase;
    }

    VirtualFree(pProcessModules, 0, MEM_RELEASE);
    return ret;
}

uintptr_t ntGetRvaCiOptions (IN void* pImageBaseKrnl) {
    path  ciPath          = getDrvDirW(L"CI.dll");
    void* pImageBase      = mapfile(ciPath);
    void* pCiOptionsInstr = scanInSection(pImageBase, "PAGE", SIG_CI_OPTIONS, sizeof(SIG_CI_OPTIONS));

    hde64s    disasm     = disasm(pCiOptionsInstr);
    uintptr_t pCiOptions = (uintptr_t)disasm.pNextInstr + (int)disasm.disp.disp32;

    return pCiOptions - (uintptr_t)pImageBase;
}
#endif // __um__

#ifdef __km__
void* (*gKeGetPrcb)(IN int processorsNumber);
PEPROCESS (*gPsGetNextProcess)(IN PEPROCESS pCurrentProccess);
void* (*gMmFreeIndependentPages)(IN void* pMem, IN size_t size);
void* (*gMmSetPageProtection)(IN void* pMem, IN size_t numberOfBytes, IN int newProtect);
void* (*gMmAllocateIndependentPagesEx)(IN size_t size, IN int numaNodeNumber, IN OPTIONAL void* pUnkown, IN OPTIONAL int unkown);
__int64 (*gRtlpInsertInvertedFunctionTableEntry)(OPTIONAL __int64 reversed, IN void* pImageBase, IN PRUNTIME_FUNCTION pFunctionTable, IN int sizeImage, IN int sizeFunctionTable);
__int64 (*gIopConnectInterruptFullySpecified)(PKINTERRUPT pKinterrupt, ULONG_PTR zero, PKSERVICE_ROUTINE pServiceRoutine, void* pServiceContext, PKSPIN_LOCK pSpinLock, DWORD vector, KIRQL irql, KIRQL synchronizeIrql, KINTERRUPT_MODE interruptMode, bool isShareVector, void* pProcessorEnableMask);

void* ntGetImageBase (IN const char* szModuleName) {
    void* ret = nullptr;

    ULONG size;
    ZwQuerySystemInformation(SystemModuleInformation, nullptr, 0, &size);

    PRTL_PROCESS_MODULES pProcessModules = (PRTL_PROCESS_MODULES)ExAllocatePool2(POOL_FLAG_PAGED, size, 'drP');
    ZwQuerySystemInformation(SystemModuleInformation, pProcessModules, size, nullptr);

    PRTL_PROCESS_MODULE_INFORMATION pModuleInfo = &pProcessModules->Modules[0];
    for (size_t i = 0; i < pProcessModules->NumberOfModules; i++, pModuleInfo++) {
        PCHAR szCurrModuleName = (PCHAR)pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;

        if (memcmp(szCurrModuleName, szModuleName, sizeof(szModuleName)) != 0)
            continue;

        ret = pModuleInfo->ImageBase;
        dbg("NtImageBase = 0x%p", ret);
    }

    ExFreePoolWithTag(pProcessModules, 'drP');
    return ret;
}

void* ntGetNtosBase () {
    return ntGetImageBase("ntoskrnl.exe");
}

void* ntMmAllocateIndependentPagesEx (IN size_t size) {
    if (!gMmAllocateIndependentPagesEx)
        gMmAllocateIndependentPagesEx = (decltype(gMmAllocateIndependentPagesEx))scanInSection(ntGetNtosBase(), "PAGE", SIG_MM_ALLOCATE_INDEPENDENT_PAGE_EX, sizeof(SIG_MM_ALLOCATE_INDEPENDENT_PAGE_EX));
    return gMmAllocateIndependentPagesEx(size, -1, nullptr, 0);
}

void ntMmFreeIndependentPages (IN void* pMem, IN size_t size) {
    if (!gMmFreeIndependentPages)
        gMmFreeIndependentPages = (decltype(gMmFreeIndependentPages))scanInSection(ntGetNtosBase(), "PAGE", SIG_MM_FREE_INDEPENDENT_PAGES, sizeof(SIG_MM_FREE_INDEPENDENT_PAGES));
    gMmFreeIndependentPages(pMem, size);
}

KIRQL ntDisableCr0WP () {
    // Если процессор поддерживает технологию защиты потока управления на основе теневых стэков, то необходимо отключить её - сброс CR4.CTE(23 бит).
    // Т.к. сброс CR0.WP при включенном CR4.CTE приводит к #GP
#ifdef ENABLE_AMD64_SHADOW_STACK
    UINT64 cr4 = __readcr4();
    __writecr4(cr4 & ~0x800000);
#endif                                    // ENABLE_SHADOW_STACK

    KIRQL oldIrql {0};
    oldIrql    = KeRaiseIrqlToDpcLevel(); // Повышаем IRQL до DPC, чтобы отключить планировщик потоков для текущего ядра(планировщик работает на DPC IRQL)
    UINT64 cr0 = __readcr0();
    __writecr0(cr0 & ~0x10000);           // Обнуляем 16-бит CR0.WP

    return oldIrql;
}

void ntEnableCr0WP (KIRQL oldIrql) {
    KeLowerIrql(oldIrql);      // восстанавливаем IRQL ядра
    UINT64 cr0 = __readcr0();
    __writecr0(cr0 | 0x10000); // Устанавливаем 16-бит CR0.WP

    // Восстановление CR4.CTE строго после поднятия CR0.WP. В противном случае #GP
#ifdef ENABLE_AMD64_SHADOW_STACK
    UINT64 cr4 = __readcr4();
    __writecr4(cr4 | 0x800000);
#endif // ENABLE_SHADOW_STACK
}

void ntDisableCr4SMAP () {
    UINT64 cr4 = __readcr4();
    __writecr4(cr4 & ~0x200000);
}

void ntEnableCr4SMAP () {
    UINT64 cr4 = __readcr4();
    __writecr4(cr4 | 0x200000);
}

void ntMmSetPageProtection (IN void* pMem, IN size_t size, IN int newProtect) {
    if (!gMmSetPageProtection)
        gMmSetPageProtection = (decltype(gMmSetPageProtection))scanInSection(ntGetNtosBase(), ".text", SIG_MM_SET_PAGE_PROTECTION, sizeof(SIG_MM_SET_PAGE_PROTECTION));
    gMmSetPageProtection(pMem, size, newProtect);
}

NTSTATUS ntIoConnectInterruptNoACPI (PKINTERRUPT* InterruptObject, PKSERVICE_ROUTINE ServiceRoutine, PVOID ServiceContext, PKSPIN_LOCK SpinLock, ULONG Vector, KIRQL Irql, KIRQL SynchronizeIrql, KINTERRUPT_MODE InterruptMode, BOOLEAN ShareVector, KAFFINITY ProcessorEnableMask, BOOLEAN FloatingSave) {
    if (!gIopConnectInterruptFullySpecified)
        gIopConnectInterruptFullySpecified = (decltype(gIopConnectInterruptFullySpecified))scanInSection(ntGetNtosBase(), "PAGE", SIG_IOP_CONNECT_INTERRUPT_FULLY_SPECIFIED, sizeof(SIG_IOP_CONNECT_INTERRUPT_FULLY_SPECIFIED));

    char saveStub[] = {
        0xE8, 0xC7, 0xC9, 0xBF, 0xFF,      // call HalGetVectorInput
        0x85, 0xC0,                        // test eax, eax
        0x0F, 0x88, 0x84, 0x00, 0x00, 0x00 // js +0x84
    };

    uintptr_t offsetHalGetVectorInput = 0xC4;
    hMemset((void*)((uintptr_t)gIopConnectInterruptFullySpecified + offsetHalGetVectorInput), 0x90, sizeof(saveStub));
    NTSTATUS result = IoConnectInterrupt(InterruptObject, ServiceRoutine, ServiceContext, SpinLock, Vector, Irql, SynchronizeIrql, InterruptMode, ShareVector, ProcessorEnableMask, FloatingSave);
    hMemcpy((void*)((uintptr_t)gIopConnectInterruptFullySpecified + offsetHalGetVectorInput), saveStub, sizeof(saveStub));
    return result;
}

__int64 ntRtlpInsertInvertedFunctionTableEntry (IN void* pImageBase, IN PRUNTIME_FUNCTION pFunctionTable, IN int sizeImage, IN int sizeFunctionTable) {
    if (!gRtlpInsertInvertedFunctionTableEntry)
        gRtlpInsertInvertedFunctionTableEntry = (decltype(gRtlpInsertInvertedFunctionTableEntry))scanInSection(ntGetNtosBase(), ".text", SIG_RTLP_INSERT_INVERTED_FUNCTION_TABLE_ENTRY, sizeof(SIG_RTLP_INSERT_INVERTED_FUNCTION_TABLE_ENTRY));
    return gRtlpInsertInvertedFunctionTableEntry(0, pImageBase, pFunctionTable, sizeImage, sizeFunctionTable);
}

PEPROCESS ntPsGetNextProcess (IN PEPROCESS pCurrentProcess) {
    if (!gPsGetNextProcess)
        gPsGetNextProcess = (decltype(gPsGetNextProcess))scanInSection(ntGetNtosBase(), "PAGE", SIG_PS_GET_NEXT_PROCESS, sizeof(SIG_PS_GET_NEXT_PROCESS));
    return gPsGetNextProcess(pCurrentProcess);
}

void ntGetMmUnloadedDrivers (OUT void** ppMmUnloadedDrivers, OUT int** pMmLastUnloadedDriver) {
    void* ntosImageBase               = ntGetImageBase("ntoskrnl.exe");
    void* pMmUnloadedDriversInstr     = scanInSection(ntosImageBase, ".text", SIG_MM_UNLOADED_DRIVERS, sizeof(SIG_MM_UNLOADED_DRIVERS));
    void* pMmLastUnloadedDriversInstr = scanInSection(ntosImageBase, ".text", SIG_MM_LAST_UNLOADED_DRIVER, sizeof(SIG_MM_LAST_UNLOADED_DRIVER));

    auto dis             = disasm(pMmUnloadedDriversInstr);
    *ppMmUnloadedDrivers = *(void**)((uintptr_t)dis.pNextInstr + dis.disp.disp32);
    dbg("pMmUnloadedDrivers = 0x%p", *ppMmUnloadedDrivers);

    auto disLast           = disasm(pMmLastUnloadedDriversInstr);
    *pMmLastUnloadedDriver = (int*)((uintptr_t)disLast.pNextInstr + disLast.disp.disp32);
    dbg("LastUnloadedDriver(0x%p) = %i", *pMmLastUnloadedDriver, **pMmLastUnloadedDriver);
}

void ntGetPiDdbCache (OUT PERESOURCE* ppLock, OUT PRTL_AVL_TABLE* ppDdbCache) {
    void* ntosImageBase = ntGetImageBase("ntoskrnl.exe");
    void* pDdbInstr     = scanInSection(ntosImageBase, "PAGE", SIG_DDB_CACHE, sizeof(SIG_DDB_CACHE));
    void* pDdbLockInstr = scanInSection(ntosImageBase, "PAGE", SIG_DDB_CACHE_LOCK, sizeof(SIG_DDB_CACHE_LOCK));
    cnull(pDdbInstr);
    cnull(pDdbLockInstr);

    auto dis    = disasm(pDdbInstr);
    *ppDdbCache = (PRTL_AVL_TABLE)((uintptr_t)dis.pNextInstr + dis.disp.disp32);
    dbg("pDdbCache = 0x%p", *ppDdbCache);

    auto disLock = disasm(pDdbLockInstr);
    *ppLock      = (PERESOURCE)((uintptr_t)disLock.pNextInstr + disLock.disp.disp32);
    dbg("pDdbLock = 0x%p", *ppLock);
}

HANDLE ntOpenFile (IN const wchar_t* szPath, IN ACCESS_MASK desiredAccess, IN ULONG shareAccess, IN ULONG openOptions) {
    UNICODE_STRING pathUnicode {0};
    RtlInitUnicodeString(&pathUnicode, szPath);

    OBJECT_ATTRIBUTES objectAttributes {0};
    InitializeObjectAttributes(&objectAttributes, &pathUnicode, OBJ_KERNEL_HANDLE, nullptr, nullptr);

    HANDLE          hResult;
    IO_STATUS_BLOCK ioStatus {0};
    cntstatus(ZwOpenFile(&hResult, desiredAccess, &objectAttributes, &ioStatus, shareAccess, openOptions));
    return hResult;
}

void ntReadFile (IN HANDLE hFile, OUT void* pOut, IN size_t size) {
    IO_STATUS_BLOCK ioStatus {0};
    cntstatus(ZwReadFile(hFile, nullptr, nullptr, nullptr, &ioStatus, pOut, size, nullptr, nullptr));
}

size_t ntGetFileSize (IN HANDLE hFile) {
    FILE_STANDARD_INFORMATION result {0};
    IO_STATUS_BLOCK           ioStatus {0};
    cntstatus(ZwQueryInformationFile(hFile, &ioStatus, &result, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation));
    return result.EndOfFile.QuadPart;
}

void* ntGetPrcb (IN int processorsNumber) {
    if (!gKeGetPrcb)
        gKeGetPrcb = (decltype(gKeGetPrcb))scanInSection(ntGetNtosBase(), ".text", SIG_KE_GET_PRCB, sizeof(SIG_KE_GET_PRCB));
    return gKeGetPrcb(processorsNumber);
}

PKPCR ntGetPrc (IN int processorsNumber) {
    return (PKPCR)((uintptr_t)ntGetPrcb(processorsNumber) - 0x180);
}

char* ntPsGetProcessImageFileName (IN PEPROCESS pProc) {
    return (char*)pProc + 0x5A8;
}

PEPROCESS ntFindProccessByName (IN const char* szProccessName) {
    PEPROCESS pCurrentProcess {0};
    pCurrentProcess = ntPsGetNextProcess(pCurrentProcess);

    while (pCurrentProcess) {
        char* szCurProcName = ntPsGetProcessImageFileName(pCurrentProcess);

        if (strlen(szCurProcName) > 0 && strcmp(szCurProcName, szProccessName) == 0) {
            ObfDereferenceObject(pCurrentProcess);
            return pCurrentProcess;
        }

        PEPROCESS pNextProcess = ntPsGetNextProcess(pCurrentProcess);
        pCurrentProcess        = pNextProcess;
    }

    return nullptr;
}
#endif // __km__
