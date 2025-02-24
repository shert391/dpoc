#include "global.h"

NTSTATUS DriverEntry (uintptr_t, uintptr_t) {
    dbg("Success map!");
    __try {
        dbg("Throw start!");
        int* p = nullptr;
        *p     = 0xDEADC0DE;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        __debugbreak();
        dbg("WHAT?!")
    }

    return STATUS_SUCCESS;
}