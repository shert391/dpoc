#include "global.h"

#define IDT_TEST
#define SIG_NT_USER_GET_POINTER_DEVICE_ORIENTATION "\x48\x89\x5C\x24\x08\x56\x57\x41\x56\x48\x83\xEC\x40\x4C\x8B\xF2\x48\x8B\xD9"

struct PACKET {
    DWORD   sig;
    PCWCHAR szText;
};

#ifdef WIN32KHOOK_TEST
__int64(NTAPI* oNtUserGetPointerDeviceOrientation)(__int64 a1, void* a2);

int64_t NTAPI hNtUserGetPointerDeviceOrientation (int64_t unused1, PACKET* pPacket) {
    if (pPacket->sig != 0xDEADC0DE)
        oNtUserGetPointerDeviceOrientation(unused1, pPacket);

    dbg("I hooked ;-( szText = %ls", pPacket->szText);
    return 0;
}
#endif // WIN32KHOOK_TEST

#ifdef IDT_TEST
BOOLEAN KserviceRoutine (IN PKINTERRUPT pInterrupt, PVOID pServiceContext) {
    dbgbp;
    ntDisableCr4SMAP();
    PACKET* pPacket = (PACKET*)pInterrupt->TrapFrame->Rcx;
    dbg("User msg: %ls", pPacket->szText);
    ntEnableCr4SMAP();
    return true;
}
#endif // IDT_TEST

NTSTATUS DriverEntry (uintptr_t, uintptr_t) {
    dbgbp;

#ifdef WIN32KHOOK_TEST
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
#endif // WIN32KHOOK_TEST

#ifdef IDT_TEST
    PKINTERRUPT pInterrupt;
    ULONG       vector = 0x37;
    ntIoConnectInterruptNoACPI(&pInterrupt, KserviceRoutine, nullptr, (PKSPIN_LOCK)0xFFFFFFFFFFFFFFFD, vector, 3, 3, Latched, false, KeQueryActiveProcessors(), false);
    for (size_t i = 0; i < KeNumberProcessors; i++) {
        PKPCR pPcr = ntGetPrc(i);

        KIDTENTRY64 idtEntry = pPcr->IdtBase[vector];
        idtEntry.Dpl         = 3;
        hMemcpy((void*)&pPcr->IdtBase[vector], &idtEntry, sizeof(KIDTENTRY64));

        dbg("pIdtEntry(0x%p) for Processor %lli DPL changed!; PKPCR = 0x%p; IsrVector = 0x%X", &pPcr->IdtBase[vector], i, pPcr, vector);
    }
#endif // IDT_TEST

    return STATUS_SUCCESS;
}