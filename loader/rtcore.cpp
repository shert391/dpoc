#include "global.h"
#include "rtcore.h"

void rtcore::read(_In_ void* pFrom, _Out_ void* pDst, _In_ size_t size) {
	rwbuff buff{ 0 };
	buff.pFrom = pFrom;
	buff.size = size;
	buff.offset = 0;

	if (!DeviceIoControl(m_hDevice, CTL_READ, &buff, sizeof(buff), &buff, sizeof(buff), nullptr, nullptr))
		error("RtCore read fail!");

	memcpy(pDst, &buff.data, size);
}

void rtcore::write(_In_ void* pFrom, _In_ void* pDst, _In_ size_t size, _Out_ void* pOld) {
	if (pOld)
		read(pDst, pOld, size);

	rwbuff buff{ 0 };
	buff.pDst = pDst;
	buff.size = size;
	buff.offset = 0;

	if (!DeviceIoControl(m_hDevice, CTL_WRITE, &buff, sizeof(buff), nullptr, 0, nullptr, nullptr))
		error("RtCore write fail!");
}