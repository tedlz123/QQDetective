/*
*	Instruction:	QQDetective Interception Open File
*	Date:			2010/5/24
*	Version:		1.0
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*	Copyright(c)	Kernone Soft 2010
*
*/


#include "..\\Headers\\QDHeader.h"

//
//ZwOpenFileHook searchs accessed directory in database,
//if it is found, return STATUS_ACCESS_DENIED, or pass it
//to ZwOpenFileReal
//

#pragma LOCKEDCODE
 
NTSTATUS ZwOpenFileHook(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    )
{
	PEPROCESS	pEprocess = PsGetCurrentProcess();
	
	if (IsInterceptionProcess(pEprocess))
	{
		__try
		{
			int i = 0;
			for (i = 0; i < ALLOWACCESS_DIRS; i++)
			{
				if (MmIsAddressValid(ObjectAttributes->ObjectName->Buffer) && MmIsAddressValid(pwsDirectory[i]))
				{
					if (wcsstr(ObjectAttributes->ObjectName->Buffer, pwsDirectory[i]))
					{
						return(ZwOpenFileReal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions));
					}
				}
			}
			
			if (pwsFileScanEvent == NULL)
			{
				pwsFileScanEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'FS');
				
				if (pwsFileScanEvent)
				{
					PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEprocess + ulProcNameOffset);
					PWSTR			pwsObjectBuffer = NULL;
					ANSI_STRING		AnsiProcessName;
					UNICODE_STRING	uniProcessName;
					
					RtlZeroMemory(pwsFileScanEvent, (MAX_PATH + 1) * sizeof(wchar_t));
					RtlInitAnsiString(&AnsiProcessName, pchProcessName);
					RtlAnsiStringToUnicodeString(&uniProcessName, &AnsiProcessName, TRUE);
					if (MmIsAddressValid(ObjectAttributes->ObjectName->Buffer))
					{
						if (wcsstr(ObjectAttributes->ObjectName->Buffer, L"\\??\\"))
						{
							//过滤“\??\”字符，因为\??\是4个宽字符，所以占用8字节
							pwsObjectBuffer = (PWSTR)((ULONG_PTR)ObjectAttributes->ObjectName->Buffer + 8);
							RtlStringCchPrintfW(pwsFileScanEvent, MAX_PATH, L"%ws;FS;%ws;TRUE;", uniProcessName.Buffer, pwsObjectBuffer);
							KdPrint(("%ws\n", pwsFileScanEvent));
						}
						
						if (pKEvtSync[QD_SYNC_EVENTNAME_FILESCAN])
						{
							KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_FILESCAN], IO_NO_INCREMENT, FALSE);
						}
						
						RtlFreeUnicodeString(&uniProcessName);
					}
				}
			}
			
			return(STATUS_ACCESS_DENIED);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			DbgPrint(("ZwOpenFile Catch exception\n"));
		}
	}
	
	return(ZwOpenFileReal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions));
}















//#pragma LOCKEDCODE

/* NTSTATUS ZwOpenFileHook(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    )
{
	PEPROCESS pEProc = PsGetCurrentProcess();
	
	if (IsInterceptionProcess(pEProc))
	{
 		NTSTATUS		ntStatus = STATUS_UNSUCCESSFUL;
		UNICODE_STRING	uniUpcaseDir;
		
		KeWaitForSingleObject(&KEvtOpenFile, Executive, KernelMode, FALSE, NULL);
		
		KdPrint(("ZwOpenFile UpcaseUnicodeString Starting!\n"));
		if (ObjectAttributes->ObjectName->Buffer != NULL)
			ntStatus = RtlUpcaseUnicodeString(&uniUpcaseDir, ObjectAttributes->ObjectName, TRUE);
		KdPrint(("ZwOpenFile UpcaseUnicodeString Finished!\n"));
		
		if (NT_SUCCESS(ntStatus))
		{
			int i = 0;
			//KdPrint(("ZwOpenFileHook RtlUpcaseUnicodeString Successed!\n")); 
			while (i < ALLOWACCESS_DIRS)
			{
				if (ObjectAttributes->ObjectName->Length > 0 && ObjectAttributes->ObjectName->Buffer != NULL && ObjectAttributes->ObjectName->MaximumLength < MAX_PATH)
				{
					if (wcsstr(uniUpcaseDir.Buffer, pwsDirName[i]) != NULL)
					{
						RtlFreeUnicodeString(&uniUpcaseDir);
						KeSetEvent(&KEvtOpenFile, IO_NO_INCREMENT, FALSE);
						return(ZwOpenFileReal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions));
					}
				}
				i++;
			}
			KdPrint(("ZwOpenFileHook Search in array finished!\n"));
			//If interception process accesses other directory, intercept it
			//KdPrint(("Inteception: %wZ\n", &uniUpcaseDir));
			if (pwsFileScanEvent == NULL)
			{
				pwsFileScanEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'FS');
				if (pwsFileScanEvent)
				{
					PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEProc + ulProcNameOffset);
					PWSTR			pwsObjectBuffer = NULL;
					ANSI_STRING		AnsiProcessName;
					UNICODE_STRING	uniProcessName;
					
					RtlZeroMemory(pwsFileScanEvent, (MAX_PATH + 1) * sizeof(wchar_t));
					RtlInitAnsiString(&AnsiProcessName, pchProcessName);
					RtlAnsiStringToUnicodeString(&uniProcessName, &AnsiProcessName, TRUE);
					if (ObjectAttributes->ObjectName->Length > 0 && ObjectAttributes->ObjectName->Buffer != NULL)
					{
						if (wcsstr(ObjectAttributes->ObjectName->Buffer, L"\\??\\"))
						{
							//过滤“\??\”字符，因为\??\是4个宽字符，所以占用8字节
							pwsObjectBuffer = (PWSTR)((ULONG_PTR)ObjectAttributes->ObjectName->Buffer + 8);
							RtlStringCchPrintfW(pwsFileScanEvent, MAX_PATH, L"%ws;FS;%ws;TRUE;", uniProcessName.Buffer, pwsObjectBuffer);
							KdPrint(("%ws\n", pwsFileScanEvent));
						}
					}
					//Signal event here
					if (pKEvtSync[QD_SYNC_EVENTNAME_FILESCAN])
					{
						KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_FILESCAN], IO_NO_INCREMENT, FALSE);
					}
					RtlFreeUnicodeString(&uniProcessName);
				}
			}
			
			RtlFreeUnicodeString(&uniUpcaseDir);
			
			KeSetEvent(&KEvtOpenFile, IO_NO_INCREMENT, FALSE);
			return(STATUS_ACCESS_DENIED);
		}
	}
	
	return(ZwOpenFileReal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions));
} */