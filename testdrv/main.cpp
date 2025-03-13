#include "global.h"

#define SIG_NT_USER_GET_POINTER_DEVICE_ORIENTATION "\x48\x89\x5C\x24\x08\x56\x57\x41\x56\x48\x83\xEC\x40\x4C\x8B\xF2\x48\x8B\xD9"

struct PACKET {
    DWORD   sig;
    PCWCHAR szText;
};

__int64(NTAPI* oNtUserGetPointerDeviceOrientation)(__int64 a1, void* a2);

int64_t NTAPI hNtUserGetPointerDeviceOrientation (int64_t unused1, PACKET* pPacket) {
    if (pPacket->sig != 0xDEADC0DE)
        oNtUserGetPointerDeviceOrientation(unused1, pPacket);

    dbg("I hooked ;-( szText = %ls", pPacket->szText);
    return 0;
}

NTSTATUS DriverEntry (uintptr_t, uintptr_t) {
    dbgbp;
    KAPC_STATE apcState {0};
    KeStackAttachProcess(ntFindProccessByName("winlogon.exe"), &apcState);

    void* pWin32kImageBase                   = ntGetImageBase("win32kbase.sys");
    void* pNtUserGetPointerDeviceOrientation = scanInSection(pWin32kImageBase, ".text", SIG_NT_USER_GET_POINTER_DEVICE_ORIENTATION, sizeof(SIG_NT_USER_GET_POINTER_DEVICE_ORIENTATION));

    hAbsolute(pNtUserGetPointerDeviceOrientation,
              hNtUserGetPointerDeviceOrientation,
              (void**)&oNtUserGetPointerDeviceOrientation,
              pWin32kImageBase,
              16);

    KeUnstackDetachProcess(&apcState);
    return STATUS_SUCCESS;
}