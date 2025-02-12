#include "amd64.h"

#ifdef __um__
physaddr getPa4lvl(_In_ void* va, _In_ physaddr pPml4Base, _In_ funcReadPa fnRead)
{
    va4lvl va4lvl(va);

    physaddr pPml4ePa = (physaddr)((uintptr_t)pPml4Base + va4lvl.pml4eIndex * 8);
    uintptr_t pml4eValue = 0;
    fnRead(pPml4Base, &pml4eValue, sizeof(uintptr_t));

    physaddr pPdpBase = (physaddr)(pml4eValue & 0xFFFFFFFFFF000); // получаем базу след.каталога путём обнуления 12-бит описания памяти: 12 бит нужно на 512 элементов таблицы каталогов/страниц
    physaddr pPdpePa = (physaddr)((uintptr_t)pPdpBase + va4lvl.pdpeIndex * 8);
    uintptr_t pdpeValue = 0;

    physaddr pPdBase = (physaddr)(pdpeValue & 0xFFFFFFFFFF000);
    physaddr pPdePa = (physaddr)((uintptr_t)pPdBase + va4lvl.pdeIndex * 8);
    uintptr_t pdeValue = 0;

    physaddr pPtBase = (physaddr)(pdeValue & 0xFFFFFFFFFF000);
    physaddr pPtePa = (physaddr)((uintptr_t)pPtBase + va4lvl.pteIndex * 8);
    uintptr_t pteValue = 0;

    uintptr_t pPfBase = pteValue & 0xFFFFFFFFFF000;
    return (physaddr)(pPfBase + va4lvl.pf4kOffset);
}
#endif