/*
*	Instruction:	QQDetective Interception Registry Scan
*	Date:			2010/5/24
*	Versioin:		1.0
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*	Copyright(c)	Kernone Soft 2010 
*/

#include "..\\Headers\\QDHeader.h"

#pragma LOCKEDCODE

NTSTATUS ZwOpenKeyHook(
	OUT PHANDLE	KeyHandle,
	IN ACCESS_MASK	DesiredAccess,
	IN POBJECT_ATTRIBUTES	ObjectAttributes
	)
{
	PEPROCESS	pEProcess = PsGetCurrentProcess();
	
	if (IsInterceptionProcess(pEProcess))
	{
		_asm int 3;
		if (DesiredAccess & STANDARD_RIGHTS_WRITE)
		{
			__try
			{
				int i = 0;
				for (i = 0; i < KEYACCESS_REG; i++)
				{
					if (MmIsAddressValid(pwsRegPath[i]) && MmIsAddressValid(ObjectAttributes->ObjectName->Buffer))
					{
						if (wcsstr(ObjectAttributes->ObjectName->Buffer, pwsRegPath[i]))
						{
							if (pwsRegScanEvent == NULL)
							{
								pwsRegScanEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'RS');
								if (pwsRegScanEvent)
								{
									ANSI_STRING		AnsiProcessName;
									UNICODE_STRING	UniProcessName;
									PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset);
									
									RtlInitAnsiString(&AnsiProcessName, pchProcessName);
									RtlAnsiStringToUnicodeString(&UniProcessName,  &AnsiProcessName, TRUE);
									
									RtlStringCchPrintfW(pwsRegScanEvent, MAX_PATH, L"%wZ;RS;%wZ;FALSE", &UniProcessName, ObjectAttributes->ObjectName);
									
									if (pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN])
									{
										KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN], IO_NO_INCREMENT, FALSE);
									}
									
									RtlFreeUnicodeString(&UniProcessName);
								}
							}
						}
					}
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgPrint("Exception catched!\n");
			}
		}
	}
	
	return(ZwOpenKeyReal(KeyHandle, DesiredAccess, ObjectAttributes));
}


// NTSTATUS ZwOpenKeyHook(
    // OUT PHANDLE  KeyHandle,
    // IN ACCESS_MASK  DesiredAccess,
    // IN POBJECT_ATTRIBUTES  ObjectAttributes
	// )
// {
	// PEPROCESS	pEProcess = PsGetCurrentProcess();
	
	// if (KeGetCurrentIrql() > PASSIVE_LEVEL)
		// return(ZwOpenKeyReal(KeyHandle, DesiredAccess, ObjectAttributes));
	
	// if (IsInterceptionProcess(pEProcess))
	// {
		// KeWaitForSingleObject(&KEvtOpenKey, Executive, KernelMode, FALSE, NULL);
		
		// if (DesiredAccess & STANDARD_RIGHTS_WRITE)
		// {
			// NTSTATUS		ntStatus;
			// UNICODE_STRING	UniRegPath;
			
			// if (ObjectAttributes->ObjectName->Buffer != NULL)
			// {
				// NTSTATUS		ntStatus;
				// UNICODE_STRING	uniRegPath;
				
				// KdPrint(("ZwOpenKey UpcaseUnicodeString Started!\n"));
 				// ntStatus = RtlUpcaseUnicodeString(&uniRegPath, ObjectAttributes->ObjectName, TRUE);
				// KdPrint(("ZwOpenKey UpcaseUnicodeString Finished!\n"));
				//KdPrint(("%wZ\n", ObjectAttributes->ObjectName));
				// if (NT_SUCCESS(ntStatus))
				// {
					// int	i = 0;
					//KdPrint(("ZwOpenKeyHook RtlUpcaseUnicodeString Successed!\n")); 
					// for (i = 0; i < KEYACCESS_REG; i++)
					// {
						// if (ObjectAttributes->ObjectName->Length > 0 && ObjectAttributes->ObjectName->Buffer != NULL && ObjectAttributes->ObjectName->MaximumLength < MAX_PATH)
						// {
							// if (wcsstr(uniRegPath.Buffer, pwsRegPath[i]) != NULL)
							// {
								// if (pwsRegScanEvent == NULL)
								// {
									// pwsRegScanEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'RS');
									// if (pwsRegScanEvent)
									// {
										// ANSI_STRING		AnsiProcessName;
										// UNICODE_STRING	UniProcessName;
										// PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset);
										
										// RtlInitAnsiString(&AnsiProcessName, pchProcessName);
										// RtlAnsiStringToUnicodeString(&UniProcessName,  &AnsiProcessName, TRUE);
										
										// RtlStringCchPrintfW(pwsRegScanEvent, MAX_PATH, L"%wZ;RS;%wZ;FALSE", &UniProcessName, ObjectAttributes->ObjectName);
										
										// if (pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN])
										// {
											// KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN], IO_NO_INCREMENT, FALSE);
										// }
										// RtlFreeUnicodeString(&UniProcessName);
									// }
								// }
							// }
						// }

					// }
					// RtlFreeUnicodeString(&uniRegPath);
				// }
			// }
		// }
		// KeSetEvent(&KEvtOpenKey, IO_NO_INCREMENT, FALSE);
	// }
	
	// return(ZwOpenKeyReal(KeyHandle, DesiredAccess, ObjectAttributes));
// } 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*  	PEPROCESS	pEProcess = PsGetCurrentProcess();
	if (IsInterceptionProcess(pEProcess))
	{
		if (DesiredAccess & STANDARD_RIGHTS_WRITE)
		{
			NTSTATUS		ntStatus;
			UNICODE_STRING	uniRegObjectName;

			ntStatus = RtlUpcaseUnicodeString(&uniRegObjectName, ObjectAttributes->ObjectName, TRUE);
			
			if (NT_SUCCESS(ntStatus))
			{
				LONG	i = 0;
				//Increment index by atom operation
				for (i = 0; i < DENIEDACCESS_REG; InterlockedIncrement(&i))
				{
					if (wcsstr(uniRegObjectName.Buffer, pwsRegPath[i]))
					{
						//If QQ accesses protected registry key
						if (pwsRegScanEvent == NULL)
						{
							pwsRegScanEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'RS');
							if (pwsRegScanEvent)
							{
								PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset);
								ANSI_STRING		AnsiProcessName;
								UNICODE_STRING	uniProcessName;
								
								RtlInitAnsiString(&AnsiProcessName, pchProcessName);
								RtlAnsiStringToUnicodeString(&uniProcessName, &AnsiProcessName, TRUE);
								
								RtlStringCchPrintfW(pwsRegScanEvent, MAX_PATH, L"%wZ;RS;%wZ;TRUE;", &uniProcessName, ObjectAttributes->ObjectName);
								RtlFreeUnicodeString(&uniProcessName);
								
								if (pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN])
								{
									KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_REGSCAN], IO_NO_INCREMENT, FALSE);
								}
								KdPrint(("%ws\n", pwsRegScanEvent));
							}
						}
						//KdPrint(("%wZ\n", &uniRegObjectName));
						//_asm int 3;
						RtlFreeUnicodeString(&uniRegObjectName);
						return(STATUS_ACCESS_DENIED);
					}
				}
				RtlFreeUnicodeString(&uniRegObjectName);
				return(ZwOpenKeyReal(KeyHandle, DesiredAccess, ObjectAttributes));
			}
		}
	}  */