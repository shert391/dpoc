#include <windows.h>
#include <iostream>
#include <cstdint>
#include "hook.h"
#include "sig.h"

#define IDT_TEST

struct PACKET {
    DWORD   sig = 0xDEADC0DE;
    PCWCHAR szText;
};

__int64(NTAPI* NtUserGetPointerDeviceOrientation)(__int64 a1, PACKET* pPacket);

int main () {
#ifdef WIN32KHOOK_TEST
    HMODULE hModule                   = LoadLibrary(L"win32u.dll");
    NtUserGetPointerDeviceOrientation = (decltype(NtUserGetPointerDeviceOrientation))GetProcAddress(hModule, "NtUserGetPointerDeviceOrientation");

    PACKET packet {0};
    packet.szText = L"Hello kernel!";

    NtUserGetPointerDeviceOrientation(0, &packet);
#endif // WIN32KHOOK_TEST
#ifdef IDT_TEST
    PACKET packet {0};
    packet.szText = L"Hello kernel!";

    __asm__ volatile(
        "int $0x37"
        :
        : "c"(&packet) // перед int кладём в RCX аргумент
    );
#endif                 // IDT_TEST

    return 0;
}
