#include "global.h"

#ifdef __um__
physaddr getPa4lvl (IN void* va, IN physaddr pPml4Base, IN funcReadPa fnRead) {
    va4lvl va4lvl(va);

    physaddr  pPml4ePa   = (physaddr)((uintptr_t)pPml4Base + va4lvl.pml4eIndex * 8);
    uintptr_t pml4eValue = 0;
    fnRead(pPml4ePa, &pml4eValue, sizeof(uintptr_t));

    physaddr  pPdpBase  = (physaddr)(pml4eValue & 0xFFFFFFFFFF000); // получаем базу след.каталога путём обнуления 12-бит описания памяти: 12 бит нужно на 512 элементов таблицы каталогов/страниц
    physaddr  pPdpePa   = (physaddr)((uintptr_t)pPdpBase + va4lvl.pdpeIndex * 8);
    uintptr_t pdpeValue = 0;
    fnRead(pPdpePa, &pdpeValue, sizeof(uintptr_t));

    physaddr  pPdBase  = (physaddr)(pdpeValue & 0xFFFFFFFFFF000);
    physaddr  pPdePa   = (physaddr)((uintptr_t)pPdBase + va4lvl.pdeIndex * 8);
    uintptr_t pdeValue = 0;
    fnRead(pPdePa, &pdeValue, sizeof(uintptr_t));

    physaddr  pPtBase  = (physaddr)(pdeValue & 0xFFFFFFFFFF000);
    physaddr  pPtePa   = (physaddr)((uintptr_t)pPtBase + va4lvl.pteIndex * 8);
    uintptr_t pteValue = 0;
    fnRead(pPtePa, &pteValue, sizeof(uintptr_t));

    uintptr_t pPfBase = pteValue & 0xFFFFFFFFFF000;
    return (physaddr)(pPfBase + va4lvl.pf4kOffset);
}
#endif