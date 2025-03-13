#include <windows.h>
#include <iostream>
#include <cstdint>
#include "hook.h"
#include "sig.h"

struct PACKET {
    DWORD   sig = 0xDEADC0DE;
    PCWCHAR szText;
};

__int64(NTAPI* NtUserGetPointerDeviceOrientation)(__int64 a1, PACKET* pPacket);

void main () {
    HMODULE hModule                   = LoadLibrary(L"win32u.dll");
    NtUserGetPointerDeviceOrientation = (decltype(NtUserGetPointerDeviceOrientation))GetProcAddress(hModule, "NtUserGetPointerDeviceOrientation");

    PACKET packet {0};
    packet.szText = L"Hello kernel!";

    NtUserGetPointerDeviceOrientation(0, &packet);
}
