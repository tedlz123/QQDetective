#include "..\\Header\\QDSystem.h"

#ifndef _QDSYSTEM_
#define	_QDSYSTEM_
QDSYSGLOBALINFO	QDSystemGlobalInfo;
#endif

void UpdateSysManRegistry()
{
	HKEY	hKey = NULL;
	HKEY	hKeyStart = NULL;

	RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\QQDetective"), 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
	RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL, 0, KEY_SET_VALUE, NULL, &hKeyStart, NULL);

	RegSetValueEx(hKey, TEXT("QLStartup"), 0, REG_DWORD, (PBYTE)&QDSystemGlobalInfo.SystemManage.bStartup, sizeof (BOOL));

	if (QDSystemGlobalInfo.SystemManage.bStartup)
	{
		TCHAR		szQLPath[MAX_PATH] = TEXT("");
		TCHAR		szPath[MAX_PATH] = TEXT("");
		DWORD	dwLen = 0;

		dwLen = GetModuleFileName(NULL, szPath, MAX_PATH);
		do
		{
			dwLen--;
		}while (szPath[dwLen] != TEXT('\\'));

		StringCchCopyN(szQLPath, MAX_PATH, szPath, dwLen);
		StringCchCat(szQLPath, MAX_PATH, TEXT("\\QDLog.exe"));

		dwLen = _tcslen(szQLPath);

		RegSetValueEx(hKeyStart, TEXT("QDLog"), 0, REG_SZ, (PBYTE)szQLPath, (dwLen + 1) * sizeof (TCHAR));
	}
	else
	{
		RegDeleteValue(hKeyStart, TEXT("QDLog"));
	}
	RegCloseKey(hKey);
}

BOOL UpdateSysCfgRegistry()
{
	HKEY	hKey = NULL;
	LONG	lResult = 0;

	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\QQDetective"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		//MessageBox(NULL, TEXT("Update Data Error!"), NULL, MB_ICONERROR | MB_OK);
		return(FALSE);
	}
	RegSetValueEx(hKey, TEXT("InterceptFileScan"), 0, REG_DWORD, (LPBYTE)&QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN], sizeof (BOOL));
	RegSetValueEx(hKey, TEXT("InterceptRegScan"), 0, REG_DWORD, (LPBYTE)&QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN], sizeof (BOOL));
	RegSetValueEx(hKey, TEXT("InterceptOpProcess"), 0, REG_DWORD, (LPBYTE)&QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS], sizeof (BOOL));
	RegSetValueEx(hKey, TEXT("InterceptCrtProcess"), 0, REG_DWORD, (LPBYTE)&QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS], sizeof (BOOL));
	
	RegCloseKey(hKey);
	return(TRUE);
}

BOOL InitGetRegCfgInfo()
{
	HKEY		hKey;
	LONG		lResult;
	PVOID		pMem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0);
	DWORD	dwBytes = 0;

	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\QQDetective"), 0, NULL, 0, KEY_QUERY_VALUE, NULL, &hKey, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		MessageBox(NULL, TEXT("Initializing configuartion information failed!"), NULL, MB_OK | MB_ICONERROR);
		return(FALSE);
	}

	lResult = RegQueryValueEx(hKey, TEXT("InterceptOpProcess"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		pMem = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMem, dwBytes);
	}

	lResult = RegQueryValueEx(hKey, TEXT("InterceptFileScan"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		HeapFree(GetProcessHeap(), 0, pMem);
		return(FALSE);
	}
	QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN] = *(PBOOL)pMem;

	lResult = RegQueryValueEx(hKey, TEXT("InterceptRegScan"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		HeapFree(GetProcessHeap(), 0, pMem);
		return(FALSE);
	}
	QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN] = *(PBOOL)pMem;

	lResult = RegQueryValueEx(hKey, TEXT("InterceptOpProcess"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		HeapFree(GetProcessHeap(), 0, pMem);
		return(FALSE);
	}
	QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS] = *(PBOOL)pMem;

	lResult = RegQueryValueEx(hKey, TEXT("InterceptCrtProcess"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		HeapFree(GetProcessHeap(), 0, pMem);
		return(FALSE);
	}
	QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS] = *(PBOOL)pMem;

	lResult = RegQueryValueEx(hKey, TEXT("QLStartup"), NULL, NULL, (PBYTE)pMem, &dwBytes);
	if (lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		HeapFree(GetProcessHeap(), 0, pMem);
		return(FALSE);
	}
	QDSystemGlobalInfo.SystemManage.bStartup = *(PBOOL)pMem;

	RegCloseKey(hKey);
	HeapFree(GetProcessHeap(), 0, pMem);
	return(TRUE);
}

BOOL InitSetDefRegCfgInfo()
{
	BOOL	bInit = FALSE;
	for (int i = INTERCEPTION_FUNC_FILESCAN; i < INTERCEPTION_FUNCNUM; i++)
	{
		QDSystemGlobalInfo.SystemConfig.bInterception[i] = TRUE;
	}

	UpdateSysManRegistry();
	bInit = UpdateSysCfgRegistry();
	return(bInit);
}

BOOL InstallDriverModule(BOOL	bShowMsg)
{
	HRSRC		hResInfo;
	HGLOBAL	hResData;
	PVOID		pvRes;
	TCHAR		szSvcName[] = TEXT("QQDetective");
	TCHAR		szSystemDir[MAX_PATH] = TEXT("");

	//
	//Creating driver file in system directory
	//
	GetSystemDirectory(szSystemDir, MAX_PATH);
	StringCchCat(szSystemDir, MAX_PATH, TEXT("\\Drivers\\QQDetective.sys"));
#ifdef _M_IX86
	hResInfo = FindResource(QDSystemGlobalInfo.SystemInfo.hSysInstance, MAKEINTRESOURCE(IDR_SYS_KSQD_X86_FREE_V1), TEXT("SYS"));
#endif

#ifdef _M_X64
	//Find x64 driver  here

#endif
	if (hResInfo != NULL)
	{
		DWORD	dwResSize;
		DWORD	dwRtn;
		HANDLE	hFileSys;

		hResData = LoadResource(QDSystemGlobalInfo.SystemInfo.hSysInstance, hResInfo);
		pvRes = LockResource(hResData);
		dwResSize = SizeofResource(QDSystemGlobalInfo.SystemInfo.hSysInstance, hResInfo);

		hFileSys = CreateFile(szSystemDir, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileSys == INVALID_HANDLE_VALUE)
			return(FALSE);

		WriteFile(hFileSys, pvRes, dwResSize, &dwRtn, NULL);
		CloseHandle(hFileSys);
		FreeResource(hResData);
	}

	//
	//Creating driver service
	//
	SC_HANDLE		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (GetLastError() == ERROR_ACCESS_DENIED)
	{
		MessageBox(NULL, TEXT("Open Service Configuration Manager Denied!\n"), NULL, MB_OK | MB_ICONERROR);
		return(FALSE);
	}

	//如果服务已经存在，启动服务
	SC_HANDLE	hSvcExist = OpenService(hSCM, TEXT("QQDetective"), SERVICE_ALL_ACCESS);
	if (hSvcExist != NULL)
	{
		//已经确认服务存在，查询服务配置状态
		SERVICE_STATUS	SvcStatus;
		QueryServiceStatus(hSvcExist, &SvcStatus);
		if (SvcStatus.dwCurrentState == SERVICE_STOPPED || SvcStatus.dwCurrentState == SERVICE_PAUSED)
		{
			StartService(hSvcExist, 0, NULL);
			while (SvcStatus.dwCurrentState != SERVICE_RUNNING)
			{
				QueryServiceStatus(hSvcExist, &SvcStatus);
				Sleep(0);
			}
		}
		CloseServiceHandle(hSvcExist);
		CloseServiceHandle(hSCM);
		return(TRUE);
	}


	SC_HANDLE		hSvc = CreateService(hSCM, szSvcName, TEXT("Kernone Detective Security Series Serivce"), SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_BOOT_START,
		SERVICE_ERROR_NORMAL, szSystemDir, NULL, NULL, NULL, NULL, NULL);

	StartService(hSvc, 0, NULL);

	SERVICE_STATUS		SvcStatus = {0};
	while (SvcStatus.dwCurrentState != SERVICE_RUNNING)
	{
		Sleep(0);
		QueryServiceStatus(hSvc, &SvcStatus);
	}

	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	return(TRUE);
}

BOOL UnInstallDriverModule(BOOL bShowMsg)
{
	TCHAR					szSystemDir[MAX_PATH] = TEXT("");
	SC_HANDLE			hSCM, hSvc;
	SERVICE_STATUS	SvcStatus = {0};

	if (bShowMsg)
	{
		if (IDNO == MessageBox(NULL, TEXT("您确认卸载（卸载之前您必须关闭“QQ侦探事件簿”，否则不能顺利卸载）？"), TEXT("卸载"), MB_YESNO | MB_ICONQUESTION))
		{
			return(FALSE);
		}
	}

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM)
	{
		hSvc = OpenService(hSCM, TEXT("QQDetective"), SERVICE_ALL_ACCESS);
		ControlService(hSvc, SERVICE_CONTROL_STOP, &SvcStatus);

		while (SvcStatus.dwCurrentState != SERVICE_STOPPED)
		{
			Sleep(0);
			QueryServiceStatus(hSvc, &SvcStatus);
		}

		DeleteService(hSvc);
		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSCM);
	}

	GetSystemDirectory(szSystemDir, MAX_PATH);
	StringCchCat(szSystemDir, MAX_PATH, TEXT("\\Drivers\\QQDetective.sys"));
	DeleteFile(szSystemDir);
	return(TRUE);
}

void ControlDriverFunc(HANDLE	hQDDev)
{
	DWORD	dwBytesRtn;
	if (QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_FILESCAN])
	{
		DeviceIoControl(hQDDev, IOCTL_QD_FILEPROTECTION_START, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}
	else
	{
		DeviceIoControl(hQDDev, IOCTL_QD_FILEPROTECTION_STOP, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}

	if (QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_REGSCAN])
	{
		DeviceIoControl(hQDDev, IOCTL_QD_REGPROTECTION_START, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}
	else
	{
		DeviceIoControl(hQDDev, IOCTL_QD_REGPROTECTION_STOP, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}

	if (QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_OPENPROCESS])
	{
		DeviceIoControl(hQDDev, IOCTL_QD_OPPROCPROTECTION_START, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}
	else
	{
		DeviceIoControl(hQDDev, IOCTL_QD_OPPROCPROTECTION_STOP, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}

	if (QDSystemGlobalInfo.SystemConfig.bInterception[INTERCEPTION_FUNC_CREATEPROCESS])
	{
		DeviceIoControl(hQDDev, IOCTL_QD_CRTPROCPROTECTION_START, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}
	else
	{
		DeviceIoControl(hQDDev, IOCTL_QD_CRTPROCPROTECTION_STOP, NULL, 0, NULL, 0, &dwBytesRtn, NULL);
	}
}

unsigned int WINAPI InitSysDlgThread(void *pvParam)
{
	DialogBox(QDSystemGlobalInfo.SystemInfo.hSysInstance, MAKEINTRESOURCE(IDD_DLG_INITUI), NULL, DlgProc_InitSystem);
	return(0);
}

void InitializeSystem()
{
	HANDLE	hQDDev = CreateFile(STRING_QDDEVNAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hQDDev == INVALID_HANDLE_VALUE)
	{
		if (!IsUserAnAdmin())
		{
			SHELLEXECUTEINFO		SEI = {sizeof (SEI)};
			TCHAR							szFilePath[MAX_PATH] = TEXT("");

			GetModuleFileName(NULL, szFilePath, MAX_PATH);
			SEI.nShow = SW_SHOWNORMAL;
			SEI.lpVerb = TEXT("RUNAS");
			SEI.lpFile = szFilePath;


			if (ShellExecuteEx(&SEI))
			{
				//If elevation privilege successed, exit process
				ExitProcess(0);
			}
			else
			{
				MessageBox(NULL, TEXT("程序需要安装拦截驱动，您必须要提升管理员权限才能继续执行！"), TEXT("提升权限"), MB_OK | MB_ICONINFORMATION);
				ExitProcess(0);
			}
		}
		else
		{
			//If driver not exists, create it

			//First, creating a dialog box to show message for user
			HANDLE hInitEvent = CreateEvent(NULL, TRUE, FALSE, QD_EVENTNAME_INITSYSTEM);
			_beginthreadex(NULL, 0, InitSysDlgThread, NULL, 0, NULL);

			//Release driver to system directory and signal event
			InstallDriverModule(TRUE);

			//If install driver successed, set global variable
			QDSystemGlobalInfo.SystemManage.bProtection = TRUE;
			SetEvent(hInitEvent);
			CloseHandle(hInitEvent);

			if (!InitSetDefRegCfgInfo())
			{
				MessageBox(NULL, TEXT("SORRY, UNKNOWN ERROR HAPPENED!"), NULL, MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
		}
	}
	else
	{
		//Driver has been installed
		if (!InitGetRegCfgInfo())
		{
			//If get registry configuartion failed
			if (!InitSetDefRegCfgInfo())
			{
				MessageBox(NULL, TEXT("SORRY, UNKNOWN ERROR HAPPENED!"), NULL, MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
		}
		//Communicate with driver and use default configuartion
		QDSystemGlobalInfo.SystemManage.bProtection = TRUE;
		ControlDriverFunc(hQDDev);
	}
	CloseHandle(hQDDev);
}