#include "global.h"

void* fFileMap (IN const wchar_t* szPath) {
#ifdef __um__
	OFSTRUCT fInfo {0};
	HANDLE	 hFile = CreateFile(szPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
	PIMAGE_NT_HEADERS64	  pNtHeaders	 = IMAGE_NT_HEADERS64(pFile);
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);

	for (size_t i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++) {
		if (!pSectionHeader->SizeOfRawData) continue;
		void*  sectionVA   = (void*)((uintptr_t)pImage + pSectionHeader->VirtualAddress);
		void*  pRawSection = (void*)((uintptr_t)pFile + pSectionHeader->PointerToRawData);
		size_t rawSize	   = pSectionHeader->SizeOfRawData;
		memcpy(sectionVA, pRawSection, rawSize);
		dbg("Section %s mapped! pRawSection = 0x%p; rawSize = %lli bytes; VA = 0x%p", pSectionHeader->Name, pRawSection, rawSize, sectionVA);
	}
}

void* fExecuteMap (IN const wchar_t* szPath) {
	void* pFile = fFileMap(szPath);

	size_t sizeImage = (IMAGE_NT_HEADERS64(pFile))->OptionalHeader.SizeOfImage;
	void*  pImage	 = VirtualAlloc(nullptr, sizeImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	cnull(pImage);

	fMapSections(pFile, pImage);

	czero(VirtualFree(pFile, 0, MEM_RELEASE));
	return pImage;
}