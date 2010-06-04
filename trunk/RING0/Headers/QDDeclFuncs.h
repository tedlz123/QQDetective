/*
 *	Instruction:Declaring Funcions
 *	Date:		2010/5/16
 *	Author:		Kernone
 *	Blog:		http://hi.baidu.com/kernone
*/

/////////////////////////////Define Macro of SSDT//////////////////////////////////
#define	SYSTEM_SREVICE(_Func)				KeServiceDescriptorTable.ServiceTableBase[*(PULONG)((PUCHAR)_Func + 1)]
#define	SYSTEM_INDEX(_Func)					(*(PULONG)((PUCHAR)_Func + 1))
#define	SYSTEM_HOOK(_Func, _Hook, _Orig)	_Orig = InterlockedExchangePointer(&_Func, _Hook)



////////////////////////////Declaring Hook Functions///////////////////////////////
NTSTATUS ZwCreateSectionHook(
	HANDLE SectionHandle, 
	ACCESS_MASK  DesiredAccess, 
	POBJECT_ATTRIBUTES ObjectAttributes, 
	PLARGE_INTEGER MaximumSize, 
	ULONG SectionPageProtection, 
	ULONG AllocationAttributes, 
	HANDLE FileHandle
	);
	
NTSTATUS ZwOpenProcessHook(
	__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess, 
	__in POBJECT_ATTRIBUTES  ObjectAttributes, 
	__in_opt PCLIENT_ID  ClientId
	);
	
NTSTATUS ZwOpenFileHook(
	OUT PHANDLE  FileHandle, 
	IN ACCESS_MASK  DesiredAccess, 
	IN POBJECT_ATTRIBUTES  ObjectAttributes, 
	OUT PIO_STATUS_BLOCK  IoStatusBlock, 
	IN ULONG  ShareAccess, 
	IN ULONG  OpenOptions
	);
	
NTSTATUS ZwOpenKeyHook(
	OUT PHANDLE  KeyHandle, 
	IN ACCESS_MASK  DesiredAccess, 
	IN POBJECT_ATTRIBUTES  ObjectAttributes
	);



////////////////////////////Declaring Project Functions////////////////////////////

ULONG GetProcNameOffset(
	);

NTSTATUS InitSystemConfig(
	);

BOOLEAN IsInterceptionProcess(
	PEPROCESS	pEProcess
	);

VOID InterceptProcess(
    IN PUNICODE_STRING  FullImageName,
    IN HANDLE  ProcessId, // where image is mapped
    IN PIMAGE_INFO  ImageInfo
    );

NTSTATUS UpdateRegistryConfig(
	ULONG ulArrayIndex, 
	BOOLEAN bStatus, 
	PWSTR pszValueName
	);

////////////////////////////Declaring Native API Function////////////////////////////
NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

NTSYSAPI
WCHAR
NTAPI
RtlAnsiCharToUnicodeChar(
    __inout PUCHAR *SourceCharacter
);
/////////////////////////////Declaring Function Define////////////////////////////////

typedef NTSTATUS (*ZWCREATESECTION)(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes  OPTIONAL,
    IN PLARGE_INTEGER  MaximumSize  OPTIONAL,
    IN ULONG  SectionPageProtection,
    IN ULONG  AllocationAttributes,
    IN HANDLE  FileHandle  OPTIONAL
); 

typedef NTSTATUS (*ZWOPENPROCESS)(
    __out PHANDLE  ProcessHandle,
    __in ACCESS_MASK  DesiredAccess,
    __in POBJECT_ATTRIBUTES  ObjectAttributes,
    __in_opt PCLIENT_ID  ClientId
);

typedef NTSTATUS (*ZWQUERYDIRECTORYFILE)(
    __in HANDLE  FileHandle,
    __in_opt HANDLE  Event,
    __in_opt PIO_APC_ROUTINE  ApcRoutine,
    __in_opt PVOID  ApcContext,
    __out PIO_STATUS_BLOCK  IoStatusBlock,
    __out PVOID  FileInformation,
    __in ULONG  Length,
    __in FILE_INFORMATION_CLASS  FileInformationClass,
    __in BOOLEAN  ReturnSingleEntry,
    __in_opt PUNICODE_STRING  FileName,
    __in BOOLEAN  RestartScan
);

typedef NTSTATUS (*ZWQUERYINFORMATIONFILE)(
    IN HANDLE  FileHandle,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    OUT PVOID  FileInformation,
    IN ULONG  Length,
    IN FILE_INFORMATION_CLASS  FileInformationClass
);

typedef NTSTATUS (*ZWSETVALUEKEY)(
    IN HANDLE  KeyHandle,
    IN PUNICODE_STRING  ValueName,
    IN ULONG  TitleIndex  OPTIONAL,
    IN ULONG  Type,
    IN PVOID  Data,
    IN ULONG  DataSize
    );

typedef NTSTATUS (*ZWOPENKEY)(
    OUT PHANDLE  KeyHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes
);

typedef NTSTATUS (*ZWOPENFILE)(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    );