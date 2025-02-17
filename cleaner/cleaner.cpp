#include "global.h"
#include "cleaner.h"

void cInit () {
	cDdbCacheTable();
	cShowDdbCacheTable();
}

void cDdbCacheTable () {
	DbgBreakPoint();

	PRTL_AVL_TABLE pDdbCache;
	PERESOURCE	   pDdbLock;
	ntGetPiDdbCache(&pDdbLock, &pDdbCache);

	PiDDBCacheEntry delEntry {0};
	RtlInitUnicodeString(&delEntry.DriverName, L"cleaner.sys");

	ExAcquireResourceExclusiveLite(pDdbLock, true);
	ctrue(RtlDeleteElementGenericTableAvl(pDdbCache, &delEntry));
	ExReleaseResourceLite(pDdbLock);
}

void cShowDdbCacheTable () {
	PRTL_AVL_TABLE pDdbCache;
	PERESOURCE	   pDdbLock;
	ntGetPiDdbCache(&pDdbLock, &pDdbCache);

	ExAcquireResourceExclusiveLite(pDdbLock, true);
	for (PiDDBCacheEntry* pElemenet = (PiDDBCacheEntry*)RtlEnumerateGenericTableAvl(pDdbCache, true);
		 pElemenet != nullptr;
		 pElemenet = (PiDDBCacheEntry*)RtlEnumerateGenericTableAvl(pDdbCache, false)) {
		wchar_t pDriverName[32] {0};
		ntUnicodeStringToWchar(&pElemenet->DriverName, pDriverName);
		dbg("(PiDDBCacheEntry*)pElemenet = 0x%p; Name = %ls; Timestamp = 0x%x; Loadstatus = %i", pElemenet, pDriverName, pElemenet->TimeDateStamp, pElemenet->LoadStatus);
	}
	ExReleaseResourceLite(pDdbLock);
}

void cMmUnloadedDrivers () {
}

void cMmLastUnloadedDrivers () {
}
