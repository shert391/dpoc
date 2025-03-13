#include "global.h"

void hAbsolute (IN void* pTarget, IN void* pDst, OUT void** ppOriginalStub, IN void* pBase, IN size_t sizeHook, IN const char* szSectionForPit) {
    char shellcode[] = {
        0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, pDst
        0xFF, 0xE0,                                                 // jmp rax
    };

    // находим свободное место в яме секции szSectionForPit
    void* pSectionPit = scanFreePit(pBase, szSectionForPit, sizeHook + sizeof(shellcode));

    // заполняем shellcode хука абсолютным указателем на функцию перенаправления
    *(uintptr_t*)(shellcode + 2) = (uintptr_t)pDst;

    // выдаём аттрибуты чтения записи для страницы захватываемой функции и ямы
    hAllowWrite(pTarget, sizeHook);
    hAllowWrite(pSectionPit, sizeHook + sizeof(shellcode));

    // сохраняем пролог функции в яме
    memcpy(pSectionPit, pTarget, sizeHook);

    // установка хука
    memset(pTarget, 0x90, sizeHook);
    memcpy(pTarget, shellcode, sizeof(shellcode));

    // возврат прежнего состояния страницы захватываемой функции
    hRestoreWrite(pTarget, sizeHook);

    // в шеллкод возврата управления кладём указатель на код после хука
    *(uintptr_t*)(shellcode + 2) = (uintptr_t)pTarget + sizeof(shellcode);

    // устанавливаем шеллкод возврата в оригинальную функцию для ямы
    memcpy((void*)((uintptr_t)pSectionPit + sizeHook), shellcode, sizeof(shellcode));

    // восстанавливаем аттрибуты страницы ямы
    hRestoreWrite(pSectionPit, sizeHook + sizeof(shellcode));

    // возвращаем указатель для вызова оригинала - это шеллкод ямы
    *ppOriginalStub = pSectionPit;
}

void hGetFreePit (const wchar_t* szSectionName) {
    ntGetImageBase("");
}

void hRestoreWrite (IN void* pTarget, IN size_t size) {
    DWORD oldProtect;

#ifdef __um__
    VirtualProtect(pTarget, size, PAGE_EXECUTE_READ, &oldProtect);
#endif
#ifdef __km__
    ntMmSetPageProtection(pTarget, size, PAGE_EXECUTE_READ);
#endif
}

void hAllowWrite (IN void* pTarget, IN size_t size) {
    DWORD oldProtect;

#ifdef __um__
    VirtualProtect(pTarget, size, PAGE_EXECUTE_READWRITE, &oldProtect);
#endif
#ifdef __km__
    ntMmSetPageProtection(pTarget, size, PAGE_EXECUTE_READWRITE);
#endif
}
