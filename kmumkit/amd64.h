﻿#pragma once
#pragma region all
typedef void* physaddr;

#define PAGE_4K_SIZE 4096    // 4KB
#define RM_SIZE      1048510 // 1MB

// нужно более строгое описание va amd64: pa для 2mb pf; pa для 1gb pf...
struct va4lvl {
    va4lvl (void* va) {
        uintptr_t constVa = (uintptr_t)(va);
        pml4eIndex        = (constVa >> 39) & 0x1FF;
        pdpeIndex         = (constVa >> 30) & 0x1FF;
        pdeIndex          = (constVa >> 21) & 0x1FF;
        pteIndex          = (constVa >> 12) & 0x1FF;
        pf4kOffset        = constVa & 0xFFF;
    }

    short pml4eIndex;
    short pdpeIndex;
    short pdeIndex;
    short pteIndex;
    short pf4kOffset;
};

typedef struct _FAR_JMP_16 {
    UCHAR  OpCode; // = 0xe9
    USHORT Offset;
} FAR_JMP_16;

typedef struct _FAR_TARGET_32 {
    ULONG  Offset;
    USHORT Selector;
} FAR_TARGET_32;

typedef struct _PSEUDO_DESCRIPTOR_32 {
    USHORT Limit;
    ULONG  Base;
} PSEUDO_DESCRIPTOR_32;

#pragma pack(pop)

typedef union _KGDTENTRY64 {
    struct {
        USHORT LimitLow;
        USHORT BaseLow;

        union {
            struct {
                UCHAR BaseMiddle;
                UCHAR Flags1;
                UCHAR Flags2;
                UCHAR BaseHigh;
            } Bytes;

            struct {
                ULONG BaseMiddle  : 8;
                ULONG Type        : 5;
                ULONG Dpl         : 2;
                ULONG Present     : 1;
                ULONG LimitHigh   : 4;
                ULONG System      : 1;
                ULONG LongMode    : 1;
                ULONG DefaultBig  : 1;
                ULONG Granularity : 1;
                ULONG BaseHigh    : 8;
            } Bits;
        };

        ULONG BaseUpper;
        ULONG MustBeZero;
    };

    ULONG64 Alignment;
} KGDTENTRY64, *PKGDTENTRY64;

typedef union _KIDTENTRY64 {
    struct {
        USHORT OffsetLow;
        USHORT Selector;
        USHORT IstIndex  : 3;
        USHORT Reserved0 : 5;
        USHORT Type      : 5;
        USHORT Dpl       : 2;
        USHORT Present   : 1;
        USHORT OffsetMiddle;
        ULONG  OffsetHigh;
        ULONG  Reserved1;
    };

    ULONG64 Alignment;
} KIDTENTRY64, *PKIDTENTRY64;

typedef union _KGDT_BASE {
    struct {
        USHORT BaseLow;
        UCHAR  BaseMiddle;
        UCHAR  BaseHigh;
        ULONG  BaseUpper;
    };

    ULONG64 Base;
} KGDT_BASE, *PKGDT_BASE;

typedef union _KGDT_LIMIT {
    struct {
        USHORT LimitLow;
        USHORT LimitHigh  : 4;
        USHORT MustBeZero : 12;
    };

    ULONG Limit;
} KGDT_LIMIT, *PKGDT_LIMIT;

#define PSB_GDT32_MAX 3

typedef struct _KDESCRIPTOR {
    USHORT Pad[3];
    USHORT Limit;
    PVOID  Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

typedef struct _KDESCRIPTOR32 {
    USHORT Pad[3];
    USHORT Limit;
    ULONG  Base;
} KDESCRIPTOR32, *PKDESCRIPTOR32;

typedef struct _KSPECIAL_REGISTERS {
    ULONG64     Cr0;
    ULONG64     Cr2;
    ULONG64     Cr3;
    ULONG64     Cr4;
    ULONG64     KernelDr0;
    ULONG64     KernelDr1;
    ULONG64     KernelDr2;
    ULONG64     KernelDr3;
    ULONG64     KernelDr6;
    ULONG64     KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    USHORT      Tr;
    USHORT      Ldtr;
    ULONG       MxCsr;
    ULONG64     DebugControl;
    ULONG64     LastBranchToRip;
    ULONG64     LastBranchFromRip;
    ULONG64     LastExceptionToRip;
    ULONG64     LastExceptionFromRip;
    ULONG64     Cr8;
    ULONG64     MsrGsBase;
    ULONG64     MsrGsSwap;
    ULONG64     MsrStar;
    ULONG64     MsrLStar;
    ULONG64     MsrCStar;
    ULONG64     MsrSyscallMask;
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

typedef struct _KPROCESSOR_STATE {
    KSPECIAL_REGISTERS SpecialRegisters;
    CONTEXT            ContextFrame;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;

typedef struct _PROCESSOR_START_BLOCK* PPROCESSOR_START_BLOCK;

typedef struct _PROCESSOR_START_BLOCK {

    //
    // The block starts with a jmp instruction to the end of the block
    //

    FAR_JMP_16 Jmp;

    //
    // Completion flag is set to non-zero when the target processor has
    // started
    //

    ULONG CompletionFlag;

    //
    // Pseudo descriptors for GDT and IDT.
    //

    PSEUDO_DESCRIPTOR_32 Gdt32;
    PSEUDO_DESCRIPTOR_32 Idt32;

    //
    // The temporary 32-bit GDT itself resides here.
    //

    KGDTENTRY64 Gdt[PSB_GDT32_MAX + 1];

    //
    // Physical address of the 64-bit top-level identity-mapped page table.
    //

    ULONG64 TiledCr3;

    //
    // Far jump target from Rm to Pm code
    //

    FAR_TARGET_32 PmTarget;

    //
    // Far jump target from Pm to Lm code
    //

    FAR_TARGET_32 LmIdentityTarget;

    //
    // Address of LmTarget
    //

    PVOID LmTarget;

    //
    // Linear address of this structure
    //

    PPROCESSOR_START_BLOCK SelfMap;

    //
    // Contents of the PAT msr
    //

    ULONG64 MsrPat;

    //
    // Contents of the EFER msr
    //

    ULONG64 MsrEFER;

    //
    // Initial processor state for the processor to be started
    //

    KPROCESSOR_STATE ProcessorState;

} PROCESSOR_START_BLOCK;

#pragma endregion

#ifdef __um__
using funcReadPa = function<void(IN physaddr pFrom, OUT void* pDst, IN size_t size)>;

physaddr getPa4lvl (IN void* va, IN physaddr pPml4Base, IN funcReadPa fnRead);
#endif // __um__