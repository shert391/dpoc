#include "global.h"
#include "cleaner.h"

void cInit () {
	cMmShowUnloadedDrivers();
	/*
	cMmUnloadedDrivers();
	dbg("---------- CLEAR ----------");
	cMmShowUnloadedDrivers();*/
}

void cDdbCacheTable () {
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

void cMmShowUnloadedDrivers () {
	int*			  pMmLastUnloadedDriver;
	PUNLOADED_DRIVERS mmUnloadedDrivers {0};
	ntGetMmUnloadedDrivers((void**)&mmUnloadedDrivers, &pMmLastUnloadedDriver);

	for (int i = 0; i < *pMmLastUnloadedDriver; i++) {
		wchar_t pDriverName[32] {0};
		ntUnicodeStringToWchar(&mmUnloadedDrivers[i].Name, pDriverName);
		dbg("PUNLOADED_DRIVERS[%i] = 0x%p; DriverName = %ls; StartAddress = 0x%p; EndAddress = 0x%p", i, &mmUnloadedDrivers[i], pDriverName, mmUnloadedDrivers[i].StartAddress, mmUnloadedDrivers[i].EndAddress);
	}
}

#define MI_UNLOADED_DRIVERS 50

void cMmUnloadedDrivers () {
	int*			  pMmLastUnloadedDriver;
	PUNLOADED_DRIVERS mmUnloadedDrivers {0};
	ntGetMmUnloadedDrivers((void**)&mmUnloadedDrivers, &pMmLastUnloadedDriver);

	UNICODE_STRING uDel;
	RtlInitUnicodeString(&uDel, L"cleaner.sys");

	for (int i = 0; i < *pMmLastUnloadedDriver; i++) {
		if (RtlCompareUnicodeString(&uDel, &mmUnloadedDrivers[i].Name, false) != 0)
			continue;
		memcpy(&mmUnloadedDrivers[i], &mmUnloadedDrivers[i + 1], sizeof(UNLOADED_DRIVERS) * (MI_UNLOADED_DRIVERS - *pMmLastUnloadedDriver));
		(*pMmLastUnloadedDriver)--;
	}
}