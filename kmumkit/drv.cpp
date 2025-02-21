#include "global.h"

#ifdef __um__
void loadDriver (IN const path& pDriverPath) {
    HKEY hKey {0};
    path regPath = L"System\\CurrentControlSet\\Services\\" / pDriverPath.filename().replace_extension();
    clstatus(RegCreateKey(HKEY_LOCAL_MACHINE, regPath.c_str(), &hKey));

    wstring objPath = wstring(L"\\??\\") + pDriverPath.wstring();
    clstatus(RegSetValueEx(hKey, L"ImagePath", 0, REG_EXPAND_SZ, (const BYTE*)objPath.c_str(), wcslen(objPath.c_str()) * sizeof(wchar_t) + 2));

    const wchar_t* szType = L"Type";
    DWORD          vType  = 1;
    RegSetValueEx(hKey, szType, 0, REG_DWORD, (PBYTE)&vType, sizeof(DWORD));

    UNICODE_STRING uFullRegPath {0};
    path           wsFullRegPath = L"\\Registry\\Machine" / regPath;
    RtlInitUnicodeString(&uFullRegPath, (wchar_t*)wsFullRegPath.c_str());

    // выдаём привилегию SeLoadDriverPrivilege текущему пользователю
    BOOLEAN oldValue;
    cntstatus(RtlAdjustPrivilege(0xA, true, false, &oldValue));

    RegCloseKey(hKey);

    NTSTATUS ntstatus = NtLoadDriver(&uFullRegPath);
    if (ntstatus == STATUS_IMAGE_ALREADY_LOADED ||
        ntstatus == STATUS_OBJECT_NAME_COLLISION)
        return;
    cntstatus(ntstatus);
}

void unloadDriver (IN const path& pDriverPath) {
    // delete reg key
    wstring wsRegPath = L"SYSTEM\\CurrentControlSet\\Services\\" / pDriverPath.filename().replace_extension();
    clstatus(RegDeleteKey(HKEY_LOCAL_MACHINE, wsRegPath.c_str()));

    // add SeLoadDriverPrivilege for user
    BOOLEAN oldValue;
    cntstatus(RtlAdjustPrivilege(0xA, true, false, &oldValue));

    // get full reg path
    UNICODE_STRING uFullRegPath {0};
    wstring        wsFullRegPath = wstring(L"\\Registry\\Machine") + wsRegPath;
    RtlInitUnicodeString(&uFullRegPath, (wchar_t*)wsFullRegPath.c_str());

    // unload
    cntstatus(NtUnloadDriver(&uFullRegPath));

    return;
}
#endif // __um__