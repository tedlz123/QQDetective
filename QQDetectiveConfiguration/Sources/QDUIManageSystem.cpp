#include "..\\Header\\QDUIManageSystem.h"


LRESULT CALLBACK WndProc_Btn(HWND hWndCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int		iCtrlID = GetWindowLongPtr(hWndCtrl, GWLP_ID), iCurIndex;
	TCHAR	szInstruction[MAX_INSTRUCTION] = TEXT("");

	iCurIndex = iCtrlID - IDC_BTN_QDOPENLOG;

	if (uMsg == WM_MOUSEMOVE)
	{
		LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_QDLOG + iCurIndex, szInstruction, MAX_INSTRUCTION);
		Static_SetText(GetDlgItem(GetParent(hWndCtrl), IDC_STATIC_INSTRUCTION), szInstruction);
	}
	switch (iCtrlID)
	{
	case IDC_BTN_QDINSTALL:
		return(BtnWndProc[iCurIndex](hWndCtrl, uMsg, wParam, lParam));
	case IDC_BTN_QDUNINSTALL:
		return(BtnWndProc[iCurIndex](hWndCtrl, uMsg, wParam, lParam));
	case IDC_BTN_QDOPENLOG:
		return(BtnWndProc[iCurIndex](hWndCtrl, uMsg, wParam, lParam));
	}
	return(0);
}

BOOL ManSysProc_OnInitPage(HWND hWndPage, HWND hWndFocus, LPARAM lParam)
{
	HWND		hWndBtnInstall = GetDlgItem(hWndPage, IDC_BTN_QDINSTALL);
	HWND		hWndBtnUninstall = GetDlgItem(hWndPage, IDC_BTN_QDUNINSTALL);
	TCHAR		szInstruction[MAX_INSTRUCTION] = TEXT("");
	
	if (!IsUserAnAdmin())
	{
		SendMessage(hWndBtnInstall, BCM_SETSHIELD, 0, (LPARAM)TRUE);
		SendMessage(hWndBtnUninstall, BCM_SETSHIELD, 0, (LPARAM)TRUE);
	}
	if (QDSystemGlobalInfo.SystemManage.bProtection)
	{
		EnableWindow(hWndBtnInstall, FALSE);
	}
	else
	{
		EnableWindow(hWndBtnUninstall, FALSE);
	}

	for (int iCtrlID = IDC_BTN_QDOPENLOG, i = 0; iCtrlID < IDC_BTN_QDUNINSTALL + 1; iCtrlID++, i++)
	{
		BtnWndProc[i] = (WNDPROC)SetWindowLongPtr(GetDlgItem(hWndPage, iCtrlID), GWLP_WNDPROC, (LONG)WndProc_Btn);
	}

	if (QDSystemGlobalInfo.SystemManage.bStartup)
		Button_SetCheck(GetDlgItem(hWndPage, IDC_CHECK_STARTUP), TRUE);

	LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_PAGEINIT, szInstruction, MAX_INSTRUCTION);
	Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);
	return(FALSE);
}

void ManSysProc_OnCommand(HWND hWndPage, int id, HWND hWndCtrl, UINT uiNotify)
{
	switch (id)
	{
	case IDC_CHECK_STARTUP:
		PropSheet_Changed(GetParent(hWndPage), hWndPage);
		break;
	case IDC_BTN_QDINSTALL:
		{
			//If successed, exit process
			//Communication with driver module
			if (IsUserAnAdmin())
			{
				QDSystemGlobalInfo.SystemManage.bProtection = TRUE;
				InstallDriverModule(TRUE);
			}
			else
			{
				SHELLEXECUTEINFO		SEI = {sizeof (SEI)};
				TCHAR							szAppPath[MAX_PATH] = TEXT("");

				GetModuleFileName(NULL, szAppPath, MAX_PATH);
				SEI.nShow = SW_SHOWNORMAL;
				SEI.lpVerb = TEXT("RUNAS");
				SEI.lpFile = szAppPath;

				if (ShellExecuteEx(&SEI))
				{
					ExitProcess(0);
				}
			}
		}
		break;
	case IDC_BTN_QDUNINSTALL:
		{
			if (IsUserAnAdmin())
			{
				if (UnInstallDriverModule(TRUE))
				{
					MessageBox(NULL, TEXT("已经卸载驱动模块，感谢您的使用！\n"), TEXT("致谢"), MB_ICONINFORMATION | MB_OK);
					ExitProcess(0);
				}
			}
			else
			{
				MessageBox(NULL, TEXT("您必须要使用管理员权限，才能卸载（不建议您卸载）"), TEXT("卸载"), MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
	case IDC_BTN_QDOPENLOG:
		{
			TCHAR		szQDLPath[MAX_PATH] = TEXT("QDLog.exe");
			DWORD	dwLen = 0;

			//dwLen = GetModuleFileName(NULL, szQDLPath,  MAX_PATH);
			//do
			//{
			//	dwLen--;
			//}while (szQDLPath[dwLen] != TEXT('\\'));

			//StringCchCat(szQDLPath, MAX_PATH, TEXT("\\QDLog.exe"));

			STARTUPINFO	StartupInfo = {0};
			PROCESS_INFORMATION	ProcInfo;

			StartupInfo.cb = sizeof (StartupInfo);
			StartupInfo.wShowWindow = SW_SHOWNORMAL;
			CreateProcess(NULL, szQDLPath, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcInfo);
			CloseHandle(ProcInfo.hThread);
			CloseHandle(ProcInfo.hProcess);

		}
		break;
	}
}

INT_PTR CALLBACK DlgProc_ManSys(HWND hWndPage, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWndPage, WM_INITDIALOG, ManSysProc_OnInitPage);
		HANDLE_MSG(hWndPage, WM_COMMAND, ManSysProc_OnCommand);
	case WM_NOTIFY:
		switch (((LPPSHNOTIFY)lParam)->hdr.code)
		{
		case PSN_APPLY:
			QDSystemGlobalInfo.SystemManage.bStartup = (BOOL)Button_GetCheck(GetDlgItem(hWndPage, IDC_CHECK_STARTUP));

			UpdateSysManRegistry();
		}
		break;
	}
	return(0);
}