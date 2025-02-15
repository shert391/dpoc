#include "global.h"
#include "cleaner.h"

void cInit () {
	cShowDdbCacheTable();
}

void cDdbCacheTable () {
}

void cShowDdbCacheTable () {
	PRTL_AVL_TABLE pDdbCache;
	PERESOURCE	   pDdbLock;
	ntGetPiDdbCache(&pDdbLock, &pDdbCache);

	for (void* pElemenet = RtlEnumerateGenericTableAvl(pDdbCache, false);
		 pElemenet != nullptr;
		 RtlEnumerateGenericTableAvl(pDdbCache, true))
		dbg("pElemenet  = 0x%p", pElemenet);
}

void cMmUnloadedDrivers () {
}

void cMmLastUnloadedDrivers () {
}
