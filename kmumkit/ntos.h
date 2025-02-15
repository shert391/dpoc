﻿#pragma once
#ifdef __um__
void*	  ntGetCiOptions ();
physaddr  ntGetPml4Base (funcReadPa fnRead);
uintptr_t ntGetRvaCiOptions (IN void* pImageBaseKrnl);
void*	  ntGetImageBase (IN const char* szModuleName);
#endif // __um__

#ifdef __km__
#define IMPORT extern "C" __declspec(dllimport)

#pragma region undock

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	HANDLE Section; // Not filled in
	PVOID  MappedBase;
	PVOID  ImageBase;
	ULONG  ImageSize;
	ULONG  Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
	ULONG						   NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef struct PiDDBCacheEntry {
	PiDDBCacheEntry* Flink;
	PiDDBCacheEntry* Blink;
	UNICODE_STRING	 DriverName;
	ULONG			 TimeDateStamp;
	NTSTATUS		 LoadStatus;
}* PPiDDBCacheEntry;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation						  = 0,
	SystemProcessorInformation					  = 1, // obsolete...delete
	SystemPerformanceInformation				  = 2,
	SystemTimeOfDayInformation					  = 3,
	SystemPathInformation						  = 4,
	SystemProcessInformation					  = 5,
	SystemCallCountInformation					  = 6,
	SystemDeviceInformation						  = 7,
	SystemProcessorPerformanceInformation		  = 8,
	SystemFlagsInformation						  = 9,
	SystemCallTimeInformation					  = 10,
	SystemModuleInformation						  = 11,
	SystemLocksInformation						  = 12,
	SystemStackTraceInformation					  = 13,
	SystemPagedPoolInformation					  = 14,
	SystemNonPagedPoolInformation				  = 15,
	SystemHandleInformation						  = 16,
	SystemObjectInformation						  = 17,
	SystemPageFileInformation					  = 18,
	SystemVdmInstemulInformation				  = 19,
	SystemVdmBopInformation						  = 20,
	SystemFileCacheInformation					  = 21,
	SystemPoolTagInformation					  = 22,
	SystemInterruptInformation					  = 23,
	SystemDpcBehaviorInformation				  = 24,
	SystemFullMemoryInformation					  = 25,
	SystemLoadGdiDriverInformation				  = 26,
	SystemUnloadGdiDriverInformation			  = 27,
	SystemTimeAdjustmentInformation				  = 28,
	SystemSummaryMemoryInformation				  = 29,
	SystemMirrorMemoryInformation				  = 30,
	SystemPerformanceTraceInformation			  = 31,
	SystemObsolete0								  = 32,
	SystemExceptionInformation					  = 33,
	SystemCrashDumpStateInformation				  = 34,
	SystemKernelDebuggerInformation				  = 35,
	SystemContextSwitchInformation				  = 36,
	SystemRegistryQuotaInformation				  = 37,
	SystemExtendServiceTableInformation			  = 38,
	SystemPrioritySeperation					  = 39,
	SystemVerifierAddDriverInformation			  = 40,
	SystemVerifierRemoveDriverInformation		  = 41,
	SystemProcessorIdleInformation				  = 42,
	SystemLegacyDriverInformation				  = 43,
	SystemCurrentTimeZoneInformation			  = 44,
	SystemLookasideInformation					  = 45,
	SystemTimeSlipNotification					  = 46,
	SystemSessionCreate							  = 47,
	SystemSessionDetach							  = 48,
	SystemSessionInformation					  = 49,
	SystemRangeStartInformation					  = 50,
	SystemVerifierInformation					  = 51,
	SystemVerifierThunkExtend					  = 52,
	SystemSessionProcessInformation				  = 53,
	SystemLoadGdiDriverInSystemSpace			  = 54,
	SystemNumaProcessorMap						  = 55,
	SystemPrefetcherInformation					  = 56,
	SystemExtendedProcessInformation			  = 57,
	SystemRecommendedSharedDataAlignment		  = 58,
	SystemComPlusPackage						  = 59,
	SystemNumaAvailableMemory					  = 60,
	SystemProcessorPowerInformation				  = 61,
	SystemEmulationBasicInformation				  = 62,
	SystemEmulationProcessorInformation			  = 63,
	SystemExtendedHandleInformation				  = 64,
	SystemLostDelayedWriteInformation			  = 65,
	SystemBigPoolInformation					  = 66,
	SystemSessionPoolTagInformation				  = 67,
	SystemSessionMappedViewInformation			  = 68,
	SystemHotpatchInformation					  = 69,
	SystemObjectSecurityMode					  = 70,
	SystemWatchdogTimerHandler					  = 71,
	SystemWatchdogTimerInformation				  = 72,
	SystemLogicalProcessorInformation			  = 73,
	SystemWow64SharedInformation				  = 74,
	SystemRegisterFirmwareTableInformationHandler = 75,
	SystemFirmwareTableInformation				  = 76,
	SystemModuleInformationEx					  = 77,
	SystemVerifierTriageInformation				  = 78,
	SystemSuperfetchInformation					  = 79,
	SystemMemoryListInformation					  = 80,
	SystemFileCacheInformationEx				  = 81,
	MaxSystemInfoClass							  = 82 // MaxSystemInfoClass should always be the last enum

} SYSTEM_INFORMATION_CLASS;

IMPORT NTSTATUS NTAPI ZwQuerySystemInformation (
	IN SYSTEM_INFORMATION_CLASS systemInformationClass,
	OUT PVOID					systemInformation,
	IN ULONG					systemInformationLength,
	OUT PULONG ReturnLength		OPTIONAL);
#pragma endregion

void  ntGetPiDdbCache (OUT PERESOURCE* ppLock, OUT PRTL_AVL_TABLE* ppDdbCache);
void* ntGetImageBase (IN const char* szModuleName);
#endif // __km__
