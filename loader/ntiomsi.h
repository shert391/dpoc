#pragma once
#include "bntiomsi.h"

#define CTL_READPA	CTL_CODE(0xC350, 0x841, METHOD_BUFFERED, FILE_READ_ACCESS)	// 0xC3506104
#define CTL_WRITEPA CTL_CODE(0xC350, 0x842, METHOD_BUFFERED, FILE_WRITE_ACCESS) // 0xC350A108

struct rwbuff {
	union {
		physaddr pFrom;
		physaddr pDst;
	};

	int blockCount;
	int blockLength;

	union {
		char unk[64];
		char wrBuff[64];
	};
};

class ntiomsi : public prov {
public:
	ntiomsi () : prov({.szDriverPath = getCurrDirW(L"ntiomsi.sys"),
					   .szSymObjPath = L"\\??\\NTIOLib_ACTIVE_X",
					   .sizeBinary	 = sizeof(g_pNtIoMsiBin),
					   .pBinary		 = g_pNtIoMsiBin}) {}

	void read (IN void* pFrom, OUT void* pDst, IN size_t size) override;
	void write (IN void* pFrom, IN void* pDst, IN size_t size, OUT void* pOld) override;

private:
	void ioreadpa (IN physaddr pFrom, OUT void* pDst, IN size_t size);
	void iowritepa (IN void* pFrom, IN physaddr pDst, IN size_t size, OUT void* pOld);
};