#pragma once
#include "brtcore.h"

#define CTL_READ  CTL_CODE(0x8000, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x80002048
#define CTL_WRITE CTL_CODE(0x8000, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x8000204C

class rtcore : public prov {
public:
    rtcore () : prov({.driverPath   = getCurrDirW(L"RTCore64.sys"),
                      .symbolicPath = L"\\??\\RtCore64",
                      .sizeBinary   = sizeof(g_pRtCoreBin),
                      .pBinary      = g_pRtCoreBin}) {
    }

    void read (IN void* pFrom, OUT void* pDst, IN size_t size) override;
    void write (IN void* pFrom, IN void* pDst, IN size_t size, OUT void* pOld) override;

private:
    struct rwbuff {
        char unk0[8];

        union {
            void* pDst;
            void* pFrom;
        };

        char unk1[4];
        int  offset;
        int  size;
        int  data;
        char unk2[16];
    };
};