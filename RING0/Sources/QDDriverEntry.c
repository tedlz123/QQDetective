/*
*	Instruction:	QQDetective Driver Entry
*	Date:			2010/5/16
*	Version:		1.0
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*	Copyright(c)	Kernone Soft 2010
*/

#include "..\\Headers\\QDHeader.h"
#pragma LOCKEDDATA
QDSYSTEMGLOBALINFO			QDSystemGlobalInfo;
__declspec(dllimport) SSDT	KeServiceDescriptorTable;


////////////////Original Function/////////////////////

ZWOPENPROCESS				ZwOpenProcessReal;
ZWCREATESECTION				ZwCreateSectionReal;
ZWOPENFILE					ZwOpenFileReal;
ZWOPENKEY					ZwOpenKeyReal;

////////////////Global Variable////////////////////////
ULONG		ulProcNameOffset;
PMDL		g_pMdl;
PULONG		g_pSSDTMapped;

//
//Database strings, hook functions intercept operation
//base on this strings
//

PSTR		pchProcName[INTERCEPTION_PROCNUMS] = {"QQ.exe", "TM.exe"};
//PWSTR		pwsDirName[ALLOWACCESS_DIRS] = {L"WINDOWS", L"INTERNET", L"SYSTEMROOT", L"TENCENT", L"QQ", L"TM", L"DEVICE", L"DOCUMENTS", L"PROGRAMDATA", L"USERS", L"TEMP", L"COMMON"};
//PWSTR		pwsDirName[ALLOWACCESS_DIRS] = {L"WINDOWS", L"Windows", L"Internet", L"SystemRoot", L"Tencent", L"QQ", L"TM", L"Device", L"Documents", L"Users", L"Temp", L"temp", L"Common"};
PWSTR		pwsRegPath[KEYACCESS_REG] = {L"Windows", L"SOFTWARE", L"software", L"Class"};
PWSTR		pwsDirectory[ALLOWACCESS_DIRS] = {L"WINDOWS", L"Windows", L"windows", L"Internet", L"System", L"system", L"User", L"Documents", L"Temp", L"TEMP", L"Device", L"Tencent", L"QQ", L"qq", L"TM", L"tm", L"Common", L".db", L"ProgramData"};


//
//Message String in here, and pass it to user
//mode process, it is unicode string
//Hook Function allocates buffer for it, and 
//device control dispatch function free it when
//it is passed to user mode process
//
PWSTR		pwsCrtProcessEvent = NULL;
PWSTR		pwsOpProcessEvent = NULL;
PWSTR		pwsFileScanEvent = NULL;
PWSTR		pwsRegScanEvent = NULL;

//
//Synchronization Event object, driver uses it to 
//signal event or reset event
//
PKEVENT		pKEvtSync[QD_SYNC_EVENTNUMS];



//
//Multiple processors synchronization
//

//KEVENT		KEvtOpenFile;
//KEVENT		KEvtOpenKey;

///////////////Module Function Declaration/////////////
DRIVER_UNLOAD 		DriverUnload;
DRIVER_INITIALIZE	DriverEntry;
DRIVER_DISPATCH		DispatchOpenDev;
DRIVER_DISPATCH		DispatchCtrlDev;

#pragma INITCODE

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryPath)
{
	PDEVICE_OBJECT		pDevObj;
	PDEVICE_EXTENSION	pDevExt;
	UNICODE_STRING		uniSymbolicLink, uniDevName;
	NTSTATUS			ntStatus;
	PMDL				pMdl;
	PULONG				pSSDTMapped;
	
	//Test bug of RtlUpcaseUnicodeString / wcsstr
	/*
	UNICODE_STRING		uniIllegal = {0}, uniUpcase;
	PWSTR				pwsStr = L"Hello", pwsNull = NULL;
	
	_asm int 3;
	
	uniIllegal.MaximumLength = 0;
	uniIllegal.Buffer = NULL;
	
	RtlUpcaseUnicodeString(&uniUpcase, &uniIllegal, TRUE);
	wcsstr(uniUpcase.Buffer, pwsStr);
	
	
	wcsstr(pwsNull, pwsStr);
	*/
	
	KdPrint(("Driver Object Address 0x%08x\n", pDriverObj));
	/////////////////////Initializing System Config/////////////////////
	RtlInitUnicodeString(&uniSymbolicLink, QDSYMBOLICLINK);
	RtlInitUnicodeString(&uniDevName, QDDEVICENAME);
	
	ulProcNameOffset = GetProcNameOffset();
	
	KdPrint(("ProcName Offset:%ld\n", ulProcNameOffset));
	
	//InitializeListHead(&ProcessesListHead);
	

	
	/////////////////////Enter DriverEntry///////////////////////////////
	
	ntStatus = IoCreateDevice(pDriverObj, sizeof (DEVICE_EXTENSION), &uniDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevObj);
	if (!NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create QQDetective Device Failed! Error Code:%d\n", ntStatus));
		return(ntStatus);
	}
	
	IoCreateSymbolicLink(&uniSymbolicLink, &uniDevName);
	pDevExt = pDevObj->DeviceExtension;
	pDevExt->pDevObj = pDevObj;
	pDevExt->uniSymLink = uniSymbolicLink;
	
	pDevObj->Flags |= DO_BUFFERED_IO;
	
	pMdl = IoAllocateMdl(KeServiceDescriptorTable.ServiceTableBase, KeServiceDescriptorTable.NumberOfServices * sizeof (ULONG_PTR),
	FALSE, FALSE, NULL);
	if (pMdl == NULL)
	{
		IoDeleteSymbolicLink(&uniSymbolicLink);
		IoDeleteDevice(pDevObj);
		return(STATUS_UNSUCCESSFUL);
	}
	
	MmBuildMdlForNonPagedPool(pMdl);
	pMdl->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	
	pSSDTMapped = (PULONG)MmMapLockedPagesSpecifyCache(pMdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
	if (pSSDTMapped == NULL)
	{
		IoDeleteSymbolicLink(&uniSymbolicLink);
		IoDeleteDevice(pDevObj);
		return(STATUS_UNSUCCESSFUL);
	}
	
	pDevExt->pMdl = pMdl;
	pDevExt->pSSDTMapped = pSSDTMapped;
	
	g_pMdl = pMdl;
	g_pSSDTMapped = pSSDTMapped;
	
	ntStatus = InitSystemConfig();
	if (!NT_SUCCESS(ntStatus))
	{
		return(ntStatus);
	}
	
	pDriverObj->DriverUnload = DriverUnload;
	pDriverObj->MajorFunction[IRP_MJ_CREATE] = 
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchOpenDev;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchCtrlDev;
	
	return(ntStatus);
}

#pragma PAGEDCODE
VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	PDEVICE_OBJECT		pDevObj;
	PDEVICE_EXTENSION	pDevExt;
	PVOID				pvAddr;
	NTSTATUS			ntStatus;
	int					i;
	
	//PAGED_CODE();
	
	pDevObj = pDriverObj->DeviceObject;
	pDevExt = pDevObj->DeviceExtension;
	
	//Restore SSDT here
	if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN])
	{
		SYSTEM_HOOK(pDevExt->pSSDTMapped[SYSTEM_INDEX(ZwOpenFile)], ZwOpenFileReal, pvAddr);
	}
	if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN])
	{
		SYSTEM_HOOK(pDevExt->pSSDTMapped[SYSTEM_INDEX(ZwOpenKey)], ZwOpenKeyReal, pvAddr);
	}
	if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS])
	{
		SYSTEM_HOOK(pDevExt->pSSDTMapped[SYSTEM_INDEX(ZwCreateSection)], ZwCreateSectionReal, pvAddr);
		//PsRemoveLoadImageNotifyRoutine(InterceptProcess);
	}
	if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS])
	{
		SYSTEM_HOOK(pDevExt->pSSDTMapped[SYSTEM_INDEX(ZwOpenProcess)], ZwOpenProcessReal, pvAddr);
	}
	
	MmUnmapLockedPages(pDevExt->pSSDTMapped, pDevExt->pMdl);
	IoFreeMdl(pDevExt->pMdl);
	
	for (i = 0; i < QD_SYNC_EVENTNUMS; i++)
	{
		if (pKEvtSync[i] != NULL)
		{
			ObDereferenceObject(pKEvtSync[i]);
		}
	}

	if (pwsFileScanEvent)
	{
		ExFreePoolWithTag(pwsFileScanEvent, 'FS');
		pwsFileScanEvent = NULL;
	}
	if (pwsRegScanEvent)
	{
		ExFreePoolWithTag(pwsRegScanEvent, 'RS');
		pwsRegScanEvent = NULL;
	}
	if (pwsOpProcessEvent)
	{
		ExFreePoolWithTag(pwsOpProcessEvent, 'OP');
		pwsOpProcessEvent = NULL;
	}
	if (pwsCrtProcessEvent)
	{
		ExFreePoolWithTag(pwsCrtProcessEvent, 'CP');
		pwsCrtProcessEvent = NULL;
	}
	
	IoDeleteSymbolicLink(&pDevExt->uniSymLink);
	IoDeleteDevice(pDevObj);
}


NTSTATUS DispatchCtrlDev(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	PIO_STACK_LOCATION	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	NTSTATUS 			ntStatus = STATUS_SUCCESS;
	ULONG				ulIoCode, ulRtnBytes;
	
	ulIoCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	switch (ulIoCode)
	{
	case IOCTL_QD_FILEPROTECTION_START:
		{
			//如果当前保护状态未启动，则Hoook SSDT
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN] == FALSE)
			{
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenFile)], ZwOpenFileHook, ZwOpenFileReal);
				KdPrint(("IOCTL: FILESCAN = TRUE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_FILESCAN, TRUE, L"InterceptFileScan");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_FILEPROTECTION_STOP:
		{
			//如果当前保护状态已经启动，则还原SSDT
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN])
			{
				PVOID	pvAddr;
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenFile)], ZwOpenFileReal, pvAddr);
				KdPrint(("IOCTL: FILESCAN = FALSE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_FILESCAN, FALSE, L"InterceptFileScan");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_REGPROTECTION_START:
		{
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN] == FALSE)
			{
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenKey)], ZwOpenKeyHook, ZwOpenKeyReal);
				KdPrint(("IOCTL: REGSCAN = TRUE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_REGSCAN, TRUE, L"InterceptRegScan");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_REGPROTECTION_STOP:
		{
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN])
			{
				PVOID	pvAddr;
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenKey)], ZwOpenKeyReal, pvAddr);
				KdPrint(("IOCTL: REGSCAN = FALSE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_REGSCAN, FALSE, L"InterceptRegScan");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_CRTPROCPROTECTION_START:
		{
			if (FALSE == QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS])
			{
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwCreateSection)], ZwCreateSectionHook, ZwCreateSectionReal);
				//PsSetLoadImageNotifyRoutine(InterceptProcess);
				KdPrint(("IOCTL: CREATEPROCESS = TRUE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_CREATEPROCESS, TRUE, L"InterceptCrtProcess");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_CRTPROCPROTECTION_STOP:
		{
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS])
			{
				PVOID	pvAddr;
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwCreateSection)], ZwCreateSectionReal, pvAddr);
				//PsRemoveLoadImageNotifyRoutine(InterceptProcess);
				KdPrint(("IOCTL: CREATEPROCESS = FALSE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_CREATEPROCESS, FALSE, L"InterceptCrtProcess");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_OPPROCPROTECTION_START:
		{
			if (FALSE == QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS])
			{
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenProcess)], ZwOpenProcessHook, ZwOpenProcessReal);
				KdPrint(("IOCTL: OPENPROCESS = TRUE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_OPENPROCESS, TRUE, L"InterceptOpProcess");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_OPPROCPROTECTION_STOP:
		{
			if (QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS])
			{
				PVOID	pvAddr;
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenProcess)], ZwOpenProcessReal, pvAddr);
				KdPrint(("IOCTL: OPENPROCESS = FALSE\n"));
			}
			ntStatus = UpdateRegistryConfig(INTERCEPTION_FUNC_OPENPROCESS, FALSE, L"InterceptOpProcess");
			ulRtnBytes = 0;
		}
		break;
	case IOCTL_QD_GETFILESCANINFO:
		{
			if (pwsFileScanEvent)
			{
				ULONG	ulLength = (wcslen(pwsFileScanEvent) + 1) * sizeof (wchar_t);
				
				if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength > 0)
				{
					RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pwsFileScanEvent, ulLength);
					ExFreePoolWithTag(pwsFileScanEvent, 'FS');
					//设置指针为NULL,标明已经传递给用户模式应用程序
					pwsFileScanEvent = NULL;
				}
				ulRtnBytes = ulLength;
				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
	case IOCTL_QD_GETREGSCANINFO:
		{
			if (pwsRegScanEvent)
			{
				ULONG	ulLength = (wcslen(pwsRegScanEvent) + 1) * sizeof (wchar_t);
				
				if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength > 0)
				{
					RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pwsRegScanEvent, ulLength);
					ExFreePoolWithTag(pwsRegScanEvent, 'RS');
					
					pwsRegScanEvent = NULL;
				}
				
				ulRtnBytes = ulLength;
				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
	case IOCTL_QD_GETOPPROCINFO:
		{
			if (pwsOpProcessEvent)
			{
				ULONG	ulLength = (wcslen(pwsOpProcessEvent) + 1) * sizeof (wchar_t);
				
				if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength > 0)
				{
					RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pwsOpProcessEvent, ulLength);
					ExFreePoolWithTag(pwsOpProcessEvent, 'OP');
					
					pwsOpProcessEvent = NULL;
				}
				
				ulRtnBytes = ulLength;
				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
	case IOCTL_QD_GETCRTPROCINFO:
		{
			if (pwsCrtProcessEvent)
			{
				ULONG	ulLength = (wcslen(pwsCrtProcessEvent) + 1) * sizeof (wchar_t);
				
				if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength > 0)
				{
					RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pwsCrtProcessEvent, ulLength);
					ExFreePoolWithTag(pwsCrtProcessEvent, 'CP');
					
					pwsCrtProcessEvent = NULL;
				}
				
				ulRtnBytes = ulLength;
				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
	case IOCTL_QD_INITSYNCEVENT:
		{
			HANDLE	hEvent[QD_SYNC_EVENTNUMS] = {0};
			int		i;
			
			for (i = 0; i < QD_SYNC_EVENTNUMS; i++)
			{
				if (pKEvtSync[i])
				{
					ObDereferenceObject(pKEvtSync[i]);
					pKEvtSync[i] = NULL;
				}
			}
			
			if (pwsFileScanEvent)
			{
				ExFreePoolWithTag(pwsFileScanEvent, 'FS');
				pwsFileScanEvent = NULL;
			}
			if (pwsRegScanEvent)
			{
				ExFreePoolWithTag(pwsRegScanEvent, 'RS');
				pwsRegScanEvent = NULL;
			}
			if (pwsOpProcessEvent)
			{
				ExFreePoolWithTag(pwsOpProcessEvent, 'OP');
				pwsOpProcessEvent = NULL;
			}
			if (pwsCrtProcessEvent)
			{
				ExFreePoolWithTag(pwsCrtProcessEvent, 'CP');
				pwsCrtProcessEvent = NULL;
			}

			RtlCopyMemory(hEvent, pIrp->AssociatedIrp.SystemBuffer, sizeof (hEvent));
			for (i = 0; i < QD_SYNC_EVENTNUMS; i++)
			{
				ntStatus = ObReferenceObjectByHandle(hEvent[i], EVENT_ALL_ACCESS, *ExEventObjectType, pIrp->RequestorMode, &pKEvtSync[i], NULL);
				if (!NT_SUCCESS(ntStatus))
				{
					ulRtnBytes = 0;
					KdPrint(("ObReferenceObjectByHandle ERROR CODE: 0x%08x\n", ntStatus));
					break;
				}
			}
			
			ulRtnBytes = 0;
			ntStatus = STATUS_SUCCESS;
		}
		break;
	default:
		ulRtnBytes = 0;
		ntStatus = STATUS_INVALID_PARAMETER;
		break;
	}
	
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = ulRtnBytes;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return(ntStatus);
}

NTSTATUS DispatchOpenDev(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return(STATUS_SUCCESS);
}



NTSTATUS InitSystemConfig()
{
	NTSTATUS			ntStatus;

	//
	//Check registry config and get configuartion information
	//
	ntStatus = RtlCheckRegistryKey(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion");
	if (!NT_SUCCESS(ntStatus))
	{
		//If the registry key not exists, create it
		ntStatus = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion");
		if (NT_SUCCESS(ntStatus))
		{
			int 	i = 0;
			PWSTR	pszValueName[INTERCEPTION_FUNCNUMS] = {L"InterceptFileScan",
			L"InterceptRegScan", L"InterceptOpProcess", L"InterceptCrtProcess"};
			
			for (i = INTERCEPTION_FUNC_FILESCAN; i < INTERCEPTION_FUNCNUMS; i++)
			{
				QDSystemGlobalInfo.QDSystemConfig.bInterception[i] = TRUE;
				RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", pszValueName[i], REG_DWORD, 
				&QDSystemGlobalInfo.QDSystemConfig.bInterception[i], sizeof (ULONG));
			}
			
			///////////////HOOK SSDT HERE/////////////////
			SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenFile)], ZwOpenFileHook, ZwOpenFileReal);
			SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenKey)], ZwOpenKeyHook, ZwOpenKeyReal);
			SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenProcess)], ZwOpenProcessHook, ZwOpenProcessReal);
			SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwCreateSection)], ZwCreateSectionHook, ZwCreateSectionReal);
			//PsSetLoadImageNotifyRoutine(InterceptProcess);

		}
		else
		{
			return(STATUS_UNSUCCESSFUL);
		}
	}
	else
	{
		//If the registry key is exists, query registry value
		//表项必须以NULL结束Name成员和QueryRoutine成员。因此，RTL_QUERY_REGISTRY_TABLE的实例必须最少是有2个元素的数组
		RTL_QUERY_REGISTRY_TABLE	QueryItem[2] = {0};
		ULONG						bRegValue, bDefValue = TRUE;
		
		QueryItem[0].QueryRoutine = NULL;
		QueryItem[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
		QueryItem[0].Name = L"InterceptFileScan";
		QueryItem[0].EntryContext = &bRegValue;
		QueryItem[0].DefaultType = REG_DWORD;
		QueryItem[0].DefaultData = &bDefValue;
		QueryItem[0].DefaultLength = sizeof (ULONG);
		
		ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", QueryItem, NULL, NULL);
		if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND)
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN] = TRUE;
			RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", L"InterceptFileScan", REG_DWORD, 
			&QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN], sizeof (ULONG));
		}
		else if (NT_SUCCESS(ntStatus))
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN] = bRegValue;
			if (bRegValue)
			{
				KdPrint(("InitSystem: FILESCAN = TURE\n"));
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenFile)], ZwOpenFileHook, ZwOpenFileReal);
			}
		}
		
		QueryItem[0].Name = L"InterceptRegScan";
		ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", QueryItem, NULL, NULL);
		if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND)
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN] = TRUE;
			RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", L"InterceptRegScan", REG_DWORD, 
			&QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN], sizeof (ULONG));
		}
		else if (NT_SUCCESS(ntStatus))
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN] = bRegValue;
			if (bRegValue)
			{
				KdPrint(("InitSystem: REGSCAN = TRUE\n"));
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenKey)], ZwOpenKeyHook, ZwOpenKeyReal);
			}
		}
		
		QueryItem[0].Name = L"InterceptOpProcess";
		ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", QueryItem, NULL, NULL);
		if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND)
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS] = TRUE;
			RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", L"InterceptOpProcess", REG_DWORD, 
			&QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS], sizeof (ULONG));
		}
		else if (NT_SUCCESS(ntStatus))
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS] = bRegValue;
			if (bRegValue)
			{
				KdPrint(("InitSystem: OPENPROCESS = TRUE\n"));
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwOpenProcess)], ZwOpenProcessHook, ZwOpenProcessReal);
			}
		}
		
		QueryItem[0].Name = L"InterceptCrtProcess";
		ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", QueryItem, NULL, NULL);
		if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND)
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS] = TRUE;
			RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", L"InterceptCrtProcess", REG_DWORD, 
			&QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS], sizeof (ULONG));
		}
		else if (NT_SUCCESS(ntStatus))
		{
			QDSystemGlobalInfo.QDSystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS] = bRegValue;
			if (bRegValue)
			{
				KdPrint(("InitSystem: CREATEPROCESS = TRUE\n"));
				SYSTEM_HOOK(g_pSSDTMapped[SYSTEM_INDEX(ZwCreateSection)], ZwCreateSectionHook, ZwCreateSectionReal);
				//PsSetLoadImageNotifyRoutine(InterceptProcess);
			}
		}
		
	}
	

	//Initialize multiple processor synchronization function event
	//KeInitializeEvent(&KEvtOpenFile, SynchronizationEvent, TRUE);
	//KeInitializeEvent(&KEvtOpenKey, SynchronizationEvent, TRUE);
	
	return(STATUS_SUCCESS);
}

#pragma LOCKEDCODE
NTSTATUS ZwOpenProcessHook(
    __out PHANDLE  ProcessHandle,
    __in ACCESS_MASK  DesiredAccess,
    __in POBJECT_ATTRIBUTES  ObjectAttributes,
    __in_opt PCLIENT_ID  ClientId
    )
{
	PEPROCESS	pEProcess;
	
	pEProcess = PsGetCurrentProcess();
	if (IsInterceptionProcess(pEProcess))
	{
		if (ClientId->UniqueProcess == PsGetCurrentProcessId())
		{
			return(ZwOpenProcessReal(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId));
		}
		else
		{
			if (pwsOpProcessEvent == NULL)
			{
				pwsOpProcessEvent = ExAllocatePoolWithTag(PagedPool, (MAX_PATH + 1) * sizeof (wchar_t), 'OP');
				if (pwsOpProcessEvent)
				{
					//Get target process information by calling PsLookupProcessByProcessId
					PEPROCESS		pETargetProcess;
					NTSTATUS		ntStatus;
					
					ntStatus = PsLookupProcessByProcessId(ClientId->UniqueProcess, &pETargetProcess);
					if (NT_SUCCESS(ntStatus))
					{
						PSTR			pchProcessName = (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset), pchTargetProcName = (PSTR)((ULONG_PTR)pETargetProcess + ulProcNameOffset);
						UNICODE_STRING	uniProcessName, uniTargetProcName;
						ANSI_STRING		AnsiProcessName, AnsiTargetProcName;

						RtlInitAnsiString(&AnsiProcessName, pchProcessName);
						RtlInitAnsiString(&AnsiTargetProcName, pchTargetProcName);
						RtlAnsiStringToUnicodeString(&uniTargetProcName, &AnsiTargetProcName, TRUE);
						RtlAnsiStringToUnicodeString(&uniProcessName, &AnsiProcessName, TRUE);
						
						RtlStringCchPrintfW(pwsOpProcessEvent, MAX_PATH, L"%wZ;OP;%wZ[PID: %ld];TRUE;", &uniProcessName, &uniTargetProcName, (LONG)ClientId->UniqueProcess);
						RtlFreeUnicodeString(&uniTargetProcName);
						RtlFreeUnicodeString(&uniProcessName);
						ObDereferenceObject(pETargetProcess);
						
						KdPrint(("%ws\n", pwsOpProcessEvent));
						
						if (pKEvtSync[QD_SYNC_EVENTNAME_OPPROC])
						{
							KeSetEvent(pKEvtSync[QD_SYNC_EVENTNAME_OPPROC], IO_NO_INCREMENT, FALSE);
						}
						KdPrint(("%ws\n", pwsOpProcessEvent));
					}
					//KdPrint(("OPENPROCESS CODE: 0x%08x\n", ntStatus));
				}
			}
			//KdPrint(("Access Other Process(PID:%ld) Intecepted!\n", (LONG)ClientId->UniqueProcess));
			return(STATUS_ACCESS_DENIED);
		}
	}
	
	return(ZwOpenProcessReal(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId));
}