#include "global.h"

__int64(NTAPI* NtUserGetPointerDeviceOrientation)(__int64 unused1, LPCWSTR pText);

int main () {
    setlocale(LC_ALL, "Russian");

    LoadLibrary(L"user32.dll");
    HMODULE hModule                   = LoadLibrary(L"win32u.dll");
    NtUserGetPointerDeviceOrientation = (decltype(NtUserGetPointerDeviceOrientation))GetProcAddress(hModule, "NtUserGetPointerDeviceOrientation");

    NtUserGetPointerDeviceOrientation(0, L"Communication Hook Test");
}