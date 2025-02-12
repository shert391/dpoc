#pragma once
#include "bntiomsi.h"

#define CTL_READPA CTL_CODE(0xC350, 0x841, METHOD_BUFFERED, FILE_READ_ACCESS) // 0xC3506104
#define CTL_WRITEPA CTL_CODE(0xC350, 0x842, METHOD_BUFFERED, FILE_WRITE_ACCESS) // 0xC350A108

struct rwbuff {
	union
	{
		physaddr pFrom;
		physaddr pDst;
	};

	int blockCount;
	int blockLength;

	union
	{
		char unk[64];
		char wrBuff[64];
	};
};

class ntiomsi : public prov {
public:
	ntiomsi() : prov({
		.szDriverPath = getCurrDirW(L"ntiomsi.sys"),
		.szSymObjPath = L"\\??\\NTIOLib_ACTIVE_X",
		.sizeBinary = sizeof(g_pNtIoMsiBin),
		.pBinary = g_pNtIoMsiBin }) {}

	void read(_In_ void* pFrom, _Out_ void* pDst, _In_ size_t size) override;
	void write(_In_ void* pFrom, _In_ void* pDst, _In_ size_t size, _Out_ void* pOld) override;

private:
	void ioreadpa(_In_ physaddr pFrom, _Out_ void* pDst, _In_ size_t size);
	void iowritepa(_In_ void* pFrom, _In_ physaddr pDst, _In_ size_t size, _Out_ void* pOld);
};