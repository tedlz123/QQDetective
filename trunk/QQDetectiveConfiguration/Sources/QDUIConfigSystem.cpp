#include "..\\Header\\QDUIConfigSystem.h"

void VerifyRank(HWND hWndTBRank)
{
	int	iInterceptionRank = 0;

	for (int i = 0; i < INTERCEPTION_FUNCNUM; i++)
	{
		if (QDSystemGlobalInfo.SystemConfig.bInterception[i])
			iInterceptionRank++;
	}

	switch (iInterceptionRank)
	{
	case INTERCEPTION_FUNCNUM:				//If all interceptions have been selected
		SendMessage(hWndTBRank, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
		break;
	case 0:														//None of interceptions have been selected
		SendMessage(hWndTBRank, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)2);
		break;
	default:														//If some interceptions have been selected
		SendMessage(hWndTBRank, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)1);
		break;
	}

}

LRESULT CALLBACK WndProc_CheckButton(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Getting control window's id, and setting static control's info
	int			iCtrlID = GetWindowLongPtr(hCtrl, GWLP_ID);
	TCHAR		szInstruction[MAX_INSTRUCTION] = TEXT("");

	if (uMsg == WM_MOUSEMOVE)
	{
		LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, iCtrlID - IDC_CHECK_FILESCAN + IDS_INSTRUCTION_FILESCAN, szInstruction, MAX_INSTRUCTION);
		Static_SetText(GetDlgItem(GetParent(hCtrl), IDC_STATIC_INSTRUCTION), szInstruction);
	}
	switch (iCtrlID)
	{
	case IDC_CHECK_FILESCAN:
		return(CheckWndProc[INTERCEPTION_FUNC_FILESCAN](hCtrl, uMsg, wParam, lParam));
	case IDC_CHECK_REGSCAN:
		return(CheckWndProc[INTERCEPTION_FUNC_REGSCAN](hCtrl, uMsg, wParam, lParam));
	case IDC_CHECK_OPENPROCESS:
		return(CheckWndProc[INTERCEPTION_FUNC_OPENPROCESS](hCtrl, uMsg, wParam, lParam));
	case IDC_CHECK_CREATEPROCESS:
		return(CheckWndProc[INTERCEPTION_FUNC_CREATEPROCESS](hCtrl, uMsg, wParam, lParam));
	}
	return(0);
}

BOOL SysCfgProc_OnInitPage(HWND hWndPage, HWND hWndFocus, LPARAM lParam)
{
	HWND		hWndTBRank = GetDlgItem(hWndPage, IDC_SLIDER_RANK);
	TCHAR		szInstruction[MAX_INSTRUCTION] = TEXT("");

	SendMessage(hWndTBRank, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 2));
	for (int iCtrlID = IDC_CHECK_FILESCAN, i = INTERCEPTION_FUNC_FILESCAN; i < INTERCEPTION_FUNCNUM; iCtrlID++, i++)
	{
		Button_SetCheck(GetDlgItem(hWndPage, iCtrlID), QDSystemGlobalInfo.SystemConfig.bInterception[i]);
		CheckWndProc[i] = (WNDPROC)SetWindowLongPtr(GetDlgItem(hWndPage, iCtrlID), GWLP_WNDPROC, (LONG)WndProc_CheckButton);
	}
	VerifyRank(hWndTBRank);

	LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_PAGEINIT, szInstruction, MAX_INSTRUCTION);
	Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);

	if (!QDSystemGlobalInfo.SystemManage.bProtection)
	{
		TCHAR		szInstruction[MAX_INSTRUCTION] = TEXT("");
		for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_FILESCAN + INTERCEPTION_FUNCNUM; i++)
		{
			EnableWindow(GetDlgItem(hWndPage, i), FALSE);
		}
		EnableWindow(hWndTBRank, FALSE);

		LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_NOSTART, szInstruction, MAX_INSTRUCTION);
		Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);
		return(FALSE);
	}
	else
	{
		for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_FILESCAN + INTERCEPTION_FUNCNUM; i++)
		{
			EnableWindow(GetDlgItem(hWndPage, i), TRUE);
		}
		EnableWindow(hWndTBRank, TRUE);
	}
	return(FALSE);
}

void SysCfgProc_OnCommand(HWND hWndPage, int id, HWND hWndCtrl, UINT uiNotify)
{
	switch (id)
	{
	case IDC_CHECK_FILESCAN:
	case IDC_CHECK_REGSCAN:
	case IDC_CHECK_OPENPROCESS:
	case IDC_CHECK_CREATEPROCESS:
		{
			int			iInterceptionRank = 0;
			HWND		hWndTBRank = GetDlgItem(hWndPage, IDC_SLIDER_RANK);

			PropSheet_Changed(GetParent(hWndPage), hWndPage);

			for (int i = 0, iCtlID = IDC_CHECK_FILESCAN; i < INTERCEPTION_FUNCNUM; i++, iCtlID++)
			{
				QDSystemGlobalInfo.SystemConfig.bInterception[i] = (BOOL)Button_GetCheck(GetDlgItem(hWndPage, iCtlID));
			}

			VerifyRank(hWndTBRank);
		}
		break;
	}
}

void SysCfgProc_OnVScroll(HWND hWndPage, HWND hWndCtrl, UINT uiNotify, int iPos)
{
	TCHAR		szInstruction[MAX_INSTRUCTION] = TEXT("");

	switch (uiNotify)
	{
	case TB_THUMBPOSITION:
	case TB_THUMBTRACK:
	case TB_BOTTOM:
	case TB_TOP:
	case TB_LINEDOWN:
	case TB_LINEUP:
	case TB_PAGEDOWN:
	case TB_PAGEUP:
	case TB_ENDTRACK:

		iPos = SendMessage(hWndCtrl, TBM_GETPOS, 0, 0);
		PropSheet_Changed(GetParent(hWndPage), hWndPage);

		switch (iPos)
		{
		case 0:					//Select all items
			for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_FILESCAN + INTERCEPTION_FUNCNUM; i++)
			{
				Button_SetCheck(GetDlgItem(hWndPage, i), TRUE);
			}

			LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_LEVEL_HIGH, szInstruction, MAX_INSTRUCTION);
			Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);
			break;
		case 1:					//Some items have  been selected
			for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_FILESCAN + INTERCEPTION_FUNCNUM; i++)
			{
				Button_SetCheck(GetDlgItem(hWndPage, i), FALSE);
			}
			for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_OPENPROCESS; i++)
			{
				Button_SetCheck(GetDlgItem(hWndPage, i), TRUE);
			}

			LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_LEVEL_MEDIUM, szInstruction, MAX_INSTRUCTION);
			Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);
			break;
		case 2:					//None of items have been selected
			for (int i = IDC_CHECK_FILESCAN; i < IDC_CHECK_FILESCAN + INTERCEPTION_FUNCNUM; i++)
			{
				Button_SetCheck(GetDlgItem(hWndPage, i), FALSE);
			}

			LoadString(QDSystemGlobalInfo.SystemInfo.hSysInstance, IDS_INSTRUCTION_LEVEL_LOW, szInstruction, MAX_INSTRUCTION);
			Static_SetText(GetDlgItem(hWndPage, IDC_STATIC_INSTRUCTION), szInstruction);
			break;
		}
		break;
	}

}

INT_PTR CALLBACK DlgProc_SysCfg(HWND hWndPage, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWndPage, WM_INITDIALOG, SysCfgProc_OnInitPage);
		HANDLE_MSG(hWndPage, WM_COMMAND, SysCfgProc_OnCommand);
		HANDLE_MSG(hWndPage, WM_VSCROLL, SysCfgProc_OnVScroll);
	case WM_NOTIFY:
		switch (((LPPSHNOTIFY)lParam)->hdr.code)
		{
		case PSN_APPLY:
			{
				for (int i = 0, iCtlID = IDC_CHECK_FILESCAN; i < INTERCEPTION_FUNCNUM; i++, iCtlID++)
				{
					QDSystemGlobalInfo.SystemConfig.bInterception[i] = (BOOL)Button_GetCheck(GetDlgItem(hWndPage, iCtlID));
				}

				//Communicating with driver module here
				HANDLE	hQDDev = CreateFile(STRING_QDDEVNAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hQDDev != INVALID_HANDLE_VALUE)
				{
					ControlDriverFunc(hQDDev);
				}
				CloseHandle(hQDDev);
				UpdateSysCfgRegistry();
			}
			break;
		}
		break;
	}
	return(0);
}

int WINAPI _tWinMain(HINSTANCE hInstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	//Initializing Here and processes some exception
	InitializeSystem();

	TCHAR	szPropSheetCaption[MAXCHARACTERNUM] = TEXT("");
	TCHAR	szPropPageSysCfg[MAXCHARACTERNUM] = TEXT("");
	TCHAR	szPropPageSysMan[MAXCHARACTERNUM] = TEXT("");
	TCHAR	szPropPageAbout[MAXCHARACTERNUM] = TEXT("");

	LoadString(hInstExe, IDS_PROPSHEET_CAPTION, szPropSheetCaption, MAXCHARACTERNUM);
	LoadString(hInstExe, IDS_PROPPAGE_SYSCFG, szPropPageSysCfg, MAXCHARACTERNUM);
	LoadString(hInstExe, IDS_PROPPAGE_SYSMAN, szPropPageSysMan, MAXCHARACTERNUM);
	LoadString(hInstExe, IDS_PROPPAGE_ABOUT, szPropPageAbout, MAXCHARACTERNUM);

	PROPSHEETPAGE			psp[QDPAGES_NUMS];
	PROPSHEETHEADER		psh;
	psp[QDPAGES_SYSCFG].dwFlags = PSP_USETITLE;
	psp[QDPAGES_SYSCFG].dwSize = sizeof (PROPSHEETPAGE);
	psp[QDPAGES_SYSCFG].hInstance = hInstExe;
	psp[QDPAGES_SYSCFG].pfnDlgProc = DlgProc_SysCfg;
	psp[QDPAGES_SYSCFG].pszTitle = szPropPageSysCfg;
	psp[QDPAGES_SYSCFG].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_SYSCFG);
	psp[QDPAGES_SYSMAN].dwFlags = PSP_USETITLE;
	psp[QDPAGES_SYSMAN].dwSize = sizeof (PROPSHEETPAGE);
	psp[QDPAGES_SYSMAN].hInstance = hInstExe;
	psp[QDPAGES_SYSMAN].pfnDlgProc = DlgProc_ManSys;
	psp[QDPAGES_SYSMAN].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_SYSMAN);
	psp[QDPAGES_SYSMAN].pszTitle = szPropPageSysMan;
	psp[QDPAGES_ABOUT].dwFlags = PSP_USETITLE;
	psp[QDPAGES_ABOUT].dwSize = sizeof (PROPSHEETPAGE);
	psp[QDPAGES_ABOUT].hInstance = hInstExe;
	psp[QDPAGES_ABOUT].pfnDlgProc = DlgProc_About;
	psp[QDPAGES_ABOUT].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_ABOUT);
	psp[QDPAGES_ABOUT].pszTitle = szPropPageAbout;

	psh.dwSize = sizeof (PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_USEHICON;
	psh.hInstance = hInstExe;
	psh.nPages = QDPAGES_NUMS;	//Modify this field later
	psh.ppsp = psp;
	psh.hwndParent = NULL;
	psh.pszCaption = szPropSheetCaption;
	psh.pszIcon = MAKEINTRESOURCE(IDI_APPICON);
	psh.hIcon = LoadIcon(hInstExe, MAKEINTRESOURCE(IDI_APPICON));

	PropertySheet(&psh);
}