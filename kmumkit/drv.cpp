#include "drv.h"

#ifdef __um__
bool loadDriver(_In_ const wchar_t* pDriverPath) {
	// create drv reg entry
	HKEY hKey{ 0 };
	wstring wsRegPath = L"SYSTEM\\CurrentControlSet\\Services\\" + path(pDriverPath).filename().replace_extension().wstring();
	clstatus(RegCreateKey(HKEY_LOCAL_MACHINE, wsRegPath.c_str(), &hKey));

	// add ImagePath value
	wstring wsObjPath = L"\\??\\" + wstring(pDriverPath);
	clstatus(RegSetValueEx(hKey, L"ImagePath", 0, REG_EXPAND_SZ, (const BYTE*)wsObjPath.c_str(), wcslen(wsObjPath.c_str()) * sizeof(wchar_t) + 2));

	// get full reg path
	UNICODE_STRING uFullRegPath{ 0 };
	wstring wsFullRegPath = wstring(L"\\Registry\\Machine") + wsRegPath;
	RtlInitUnicodeString(&uFullRegPath, (wchar_t*)wsFullRegPath.c_str());

	// add SeLoadDriverPrivilege for user
	BOOLEAN oldValue;
	cntstatus(RtlAdjustPrivilege(0xA, true, false, &oldValue));

	// close reg key
	RegCloseKey(hKey);

	// load driver
	NTSTATUS ntstatus = NtLoadDriver(&uFullRegPath);
	if (ntstatus == STATUS_IMAGE_ALREADY_LOADED ||
		ntstatus == STATUS_OBJECT_NAME_COLLISION) {
		return true;
	}
	cntstatus(ntstatus);
}

bool unloadDriver(_In_ const wchar_t* pDriverPath)
{
	// delete reg key
	wstring wsRegPath = L"SYSTEM\\CurrentControlSet\\Services\\" + path(pDriverPath).filename().replace_extension().wstring();
	clstatus(RegDeleteKey(HKEY_LOCAL_MACHINE, wsRegPath.c_str()));

	// add SeLoadDriverPrivilege for user
	BOOLEAN oldValue;
	cntstatus(RtlAdjustPrivilege(0xA, true, false, &oldValue));

	// get full reg path
	UNICODE_STRING uFullRegPath{ 0 };
	wstring wsFullRegPath = wstring(L"\\Registry\\Machine") + wsRegPath;
	RtlInitUnicodeString(&uFullRegPath, (wchar_t*)wsFullRegPath.c_str());

	// unload
	cntstatus(NtUnloadDriver(&uFullRegPath));

	return true;
}
#endif // __um__