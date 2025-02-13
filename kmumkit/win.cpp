#include "win.h"

void* mapfile (const wchar_t* szPath) {
	HANDLE hFile = CreateFileW(szPath,
	                           GENERIC_READ,
	                           0,
	                           nullptr,
	                           OPEN_EXISTING,
	                           0,
	                           nullptr);
	chandle(hFile);

	HANDLE hSection = CreateFileMappingA(hFile,
	                                     nullptr,
	                                     PAGE_READONLY | SEC_IMAGE,
	                                     0,
	                                     0,
	                                     nullptr);
	chandle(hSection);

	void* pFileMap = MapViewOfFile(hSection, FILE_MAP_READ, 0, 0, 0);

	CloseHandle(hSection);
	CloseHandle(hFile);

	return pFileMap;
}
