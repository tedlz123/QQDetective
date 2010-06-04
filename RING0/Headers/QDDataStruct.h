/*
*	Instruction:	QQDetective SYSTEM DATA STRUCTURE
*	Date:			2010/5/16
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*/

#ifndef _QD_DATA_STRUCTURE
#define	_QD_DATA_STRUCTURE
/////////////////////////////QQDective Data Structure///////////////////////
#define		SEC_IMAGE	0x1000000
#define		MAX_PATH	320
#define		ALLOWACCESS_DIRS						19
#define		KEYACCESS_REG							4
#define		INTERCEPTION_PROCNUMS					2

//
//Defining SynchrnoizatioinEvent number, and user mode
//pass same number to driver for indicating event type
//
#define	QD_SYNC_EVENTNUMS					4
#define	QD_SYNC_EVENTNAME_FILESCAN			0
#define	QD_SYNC_EVENTNAME_REGSCAN			1
#define	QD_SYNC_EVENTNAME_OPPROC			2
#define	QD_SYNC_EVENTNAME_CRTPROC			3

//Define Interception Nubmer
#define		INTERCEPTION_FUNCNUMS					4
#define		INTERCEPTION_FUNC_FILESCAN				0
#define		INTERCEPTION_FUNC_REGSCAN				1
#define		INTERCEPTION_FUNC_OPENPROCESS			2
#define		INTERCEPTION_FUNC_CREATEPROCESS			3

typedef struct _QD_SYSTEM_CONFIG
{
	ULONG	bInterception[INTERCEPTION_FUNCNUMS];
}QDSYSTEMCONFIG, *PQDSYSTEMCONFIG;

typedef struct _QD_SYSTEM_GLOBAL_INFO
{
	QDSYSTEMCONFIG	QDSystemConfig;
}QDSYSTEMGLOBALINFO, *PQDSYSTEMGLOBALINFO;


typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT	pDevObj;
	UNICODE_STRING	uniSymLink;
	PMDL			pMdl;
	PULONG			pSSDTMapped;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//////////////////////////////SYSTEM DATA STRUCT////////////////////////////////

#pragma pack(1)
typedef struct _SYSTEM_SERVICE_DESCRIPTOR_TABLE
{
	PULONG				ServiceTableBase;
	PULONG				ServiceCounterTableBase;
	unsigned int		NumberOfServices;
	PULONG				ParamTableBase;
}SSDT, *PSSDT;
#pragma pack()

typedef enum _SYSTEM_INFORMATION_CLASS { 
	SystemBasicInformation, 				// 0 
	SystemProcessorInformation, 			// 1 
	SystemPerformanceInformation, 			// 2
	SystemTimeOfDayInformation, 			// 3
	SystemNotImplemented1, 				// 4
	SystemProcessesAndThreadsInformation, 		// 5
	SystemCallCounts, 					// 6
	SystemConfigurationInformation, 			// 7
	SystemProcessorTimes, 				// 8
	SystemGlobalFlag, 					// 9
	SystemNotImplemented2, 				// 10
	SystemModuleInformation, 				// 11
	SystemLockInformation, 				// 12
	SystemNotImplemented3, 				// 13
	SystemNotImplemented4, 				// 14
	SystemNotImplemented5, 				// 15
	SystemHandleInformation, 				// 16
	SystemObjectInformation, 				// 17
	SystemPagefileInformation, 				// 18
	SystemInstructionEmulationCounts, 			// 19
	SystemInvalidInfoClass1, 				// 20
	SystemCacheInformation, 				// 21
	SystemPoolTagInformation, 				// 22
	SystemProcessorStatistics, 				// 23
	SystemDpcInformation, 				// 24
	SystemNotImplemented6, 				// 25
	SystemLoadImage, 					// 26
	SystemUnloadImage, 				// 27
	SystemTimeAdjustment, 				// 28
	SystemNotImplemented7, 				// 29
	SystemNotImplemented8, 				// 30
	SystemNotImplemented9, 				// 31
	SystemCrashDumpInformation, 			// 32
	SystemExceptionInformation, 			// 33
	SystemCrashDumpStateInformation, 			// 34
	SystemKernelDebuggerInformation, 			// 35
	SystemContextSwitchInformation, 			// 36
	SystemRegistryQuotaInformation, 			// 37
	SystemLoadAndCallImage, 				// 38
	SystemPrioritySeparation, 				// 39
	SystemNotImplemented10, 				// 40
	SystemNotImplemented11, 				// 41
	SystemInvalidInfoClass2, 				// 42
	SystemInvalidInfoClass3, 				// 43
	SystemTimeZoneInformation, 				// 44
	SystemLookasideInformation, 			// 45
	SystemSetTimeSlipEvent, 				// 46
	SystemCreateSession, 				// 47
	SystemDeleteSession, 				// 48
	SystemInvalidInfoClass4, 				// 49
	SystemRangeStartInformation, 			// 50
	SystemVerifierInformation, 				// 51
	SystemAddVerifier, 				// 52
	SystemSessionProcessesInformation 			// 53
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_THREAD_INFORMATION {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	LONG State;
	LONG WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters;
	SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

#endif