#include "global.h"

void rtcore::read (IN void* pFrom, OUT void* pDst, IN size_t size) {
	rwbuff buff {0};
	buff.pFrom	= pFrom;
	buff.size	= size;
	buff.offset = 0;

	if (!DeviceIoControl(m_hDevice, CTL_READ, &buff, sizeof(buff), &buff, sizeof(buff), nullptr, nullptr))
		error("RtCore read fail!");

	memcpy(pDst, &buff.data, size);
}

void rtcore::write (IN void* pFrom, IN void* pDst, IN size_t size, OUT void* pOld) {
	if (pOld)
		read(pDst, pOld, size);

	rwbuff buff {0};
	buff.pDst	= pDst;
	buff.size	= size;
	buff.offset = 0;

	if (!DeviceIoControl(m_hDevice, CTL_WRITE, &buff, sizeof(buff), nullptr, 0, nullptr, nullptr))
		error("RtCore write fail!");
}