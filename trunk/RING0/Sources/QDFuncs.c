#include "..\\Headers\QDHeader.h"

#pragma LOCKEDCODE
BOOLEAN IsInterceptionProcess(PEPROCESS	pEProcess)
{
	ANSI_STRING		AnsiDataBaseName;
	ANSI_STRING		AnsiAnalysisName;
	unsigned int	i = 0;
	
	RtlInitAnsiString(&AnsiAnalysisName, (PSTR)((ULONG_PTR)pEProcess + ulProcNameOffset));
	for (i = 0; i < INTERCEPTION_PROCNUMS; i++)
	{
		RtlInitAnsiString(&AnsiDataBaseName, (PSTR)pchProcName[i]);
		if (0 == RtlCompareString(&AnsiDataBaseName, &AnsiAnalysisName, TRUE))
		{
			return(TRUE);
		}
	}
	return(FALSE);
}

ULONG GetProcNameOffset()
{
	ULONG		ulOffset = 0;
	PEPROCESS	pEProc = PsGetCurrentProcess();
	PSTR		pszProcName = NULL;
	PSTR		pszSysName = "System";
	
	//PAGED_CODE();
	
	while (ulOffset < 4096)
	{
		pszProcName = (PSTR)((ULONG_PTR)pEProc + ulOffset);
		if (0 == strcmp(pszProcName, pszSysName))
			break;
		
		ulOffset++;
	}

	return(ulOffset);
}

NTSTATUS UpdateRegistryConfig(ULONG ulArrayIndex, BOOLEAN bStatus, PWSTR pszValueName)
{
	NTSTATUS	ntStatus;
	
	QDSystemGlobalInfo.QDSystemConfig.bInterception[ulArrayIndex] = bStatus;
	ntStatus = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, L"QQDetective\\Configuartion", pszValueName, REG_DWORD, 
	&QDSystemGlobalInfo.QDSystemConfig.bInterception[ulArrayIndex], sizeof (ULONG));
	
	//KdPrint(("UpdateRegistryConfig: %08x\n", ntStatus));
	return(ntStatus);
}


