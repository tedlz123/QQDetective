/*
*	Instruction:	QQDetective Inteception Create Process
*	Date:			2010/5/16
*	Version:		1.0
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*	Copyright(c)	Kernone Soft 2010
*/

#include "..\\Headers\\QDHeader.h"
//
//Interception create process by malware process, by getting
//parent process id to ensure whether intercept it or not
//

#pragma LOCKEDCODE
NTSTATUS ZwCreateSectionHook(HANDLE SectionHandle, ACCESS_MASK  DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PLARGE_INTEGER MaximumSize, ULONG SectionPageProtection, ULONG AllocationAttributes, HANDLE FileHandle)
{
	
	if (/*(DesiredAccess & SECTION_MAP_EXECUTE) && */(AllocationAttributes & SEC_IMAGE) && (SectionPageProtection & PAGE_EXECUTE))
	{
		PEPROCESS					pEParent = NULL;
		PROCESS_BASIC_INFORMATION	ProcInfo = {0};
		NTSTATUS					ntStatus = 0;
		ULONG						ulRtn;
		HANDLE						hProcess;
		CLIENT_ID					ProcID = {0};
		OBJECT_ATTRIBUTES			objAttr;
		
		ProcID.UniqueProcess = PsGetCurrentProcessId();
		InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
		
		ntStatus = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &ProcID);
		if (NT_SUCCESS(ntStatus))
			ntStatus = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &ProcInfo, sizeof (ProcInfo), &ulRtn);
		
		
		if (NT_SUCCESS(ntStatus))
		{
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ProcInfo.InheritedFromUniqueProcessId, &pEParent)))
			{
				

				if (IsInterceptionProcess(pEParent))
				{
					//_asm int 3;
					//KdPrint(("ZwCreateSection Found!!!\n"));
					//
					//Update information of pwsCrtProcessEvent
					//
					if (pwsCrtProcessEvent == NULL)
					{
						pwsCrtProcessEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'CP');
						if (pwsCrtProcessEvent)
						{
							PEPROCESS		pEProcess = PsGetCurrentProcess();
							PSTR			pchParentName = (PSTR)((ULONG_PTR)pEParent + ulProcNameOffset), pchProcessName = (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset);
							ANSI_STRING		AnsiParentName, AnsiProcessName;
							UNICODE_STRING	uniParentName, uniProcessName;
							ULONG			ulPos = 0;
							
							//_asm int 3;
							
							RtlZeroMemory(pwsCrtProcessEvent, (MAX_PATH + 1) * sizeof (wchar_t));

							RtlInitAnsiString(&AnsiParentName, pchParentName);
							RtlInitAnsiString(&AnsiProcessName, pchProcessName);
							
							RtlAnsiStringToUnicodeString(&uniParentName, &AnsiParentName, TRUE);
							RtlAnsiStringToUnicodeString(&uniProcessName, &AnsiProcessName, TRUE);
							
							RtlStringCchPrintfW(pwsCrtProcessEvent, MAX_PATH, L"%ws;CP;%ws[PID: %ld];TRUE;", uniParentName.Buffer, uniProcessName.Buffer, (ULONG)PsGetCurrentProcessId());
							
							RtlFreeUnicodeString(&uniProcessName);
							RtlFreeUnicodeString(&uniParentName);
							
							if (pKEvtSync[QD_SYNC_EVENTNAME_CRTPROC])
							{
								KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_CRTPROC], IO_NO_INCREMENT, FALSE);
							}
							KdPrint(("%S\n", pwsCrtProcessEvent));
						}
					}
					ObDereferenceObject(pEParent);
					ZwTerminateProcess(hProcess, 0);
					ZwClose(hProcess);
					//KdPrint(("ZwCreateSection Found return.\n"));
					return(0);
				}

				ObDereferenceObject(pEParent);
			}
		}
		ZwClose(hProcess);

	}
	
	return(ZwCreateSectionReal(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle));
}

//
//InteceptProcess is callback notify routine that be set by
//PsSetLoadImageNotifyRoutine kernel function
//
VOID InterceptProcess(
    IN PUNICODE_STRING  FullImageName,
    IN HANDLE  ProcessId, // where image is mapped
    IN PIMAGE_INFO  ImageInfo
    )
{
	NTSTATUS			ntStatus;
	OBJECT_ATTRIBUTES	objAttr;
	CLIENT_ID			ClientId = {0};
	HANDLE				hCurProcess;
	
	ClientId.UniqueProcess = ProcessId;
	InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
	
	ntStatus = ZwOpenProcess(&hCurProcess, PROCESS_ALL_ACCESS, &objAttr, &ClientId);
	if (NT_SUCCESS(ntStatus))
	{
		PROCESS_BASIC_INFORMATION	PBICurProc = {0};
		ULONG						ulRtn;
				  // NtQueryInformationProcess(hProcess, ProcessBasicInformation, &ProcInfo, sizeof (ProcInfo), &ulRtn);
		ntStatus = NtQueryInformationProcess(hCurProcess, ProcessBasicInformation, &PBICurProc, sizeof (PBICurProc), &ulRtn);
		KdPrint(("LoadImageNotify: NtQueryInformation ERROR CODE: 0x%08x\n", ntStatus));
		if (NT_SUCCESS(ntStatus))
		{
			PEPROCESS	pEParent;

			KdPrint(("LoadImageNotify: NtQueryInformationProcess Successfully!\n"));
			ntStatus = PsLookupProcessByProcessId((HANDLE)PBICurProc.InheritedFromUniqueProcessId, &pEParent);
			if (NT_SUCCESS(ntStatus))
			{
				if (IsInterceptionProcess(pEParent))
				{
					KdPrint(("This Process's parent process is TENCENT IM\n"));

					ZwTerminateProcess(hCurProcess, 0);
				}
				ObDereferenceObject(pEParent);
			}
			
		}
		
		
		ZwClose(hCurProcess);
	}
	
}