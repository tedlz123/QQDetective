#include "QDLHeader.h"

QDLGLOBALINFO	QDLGlobalInfo;
void OneInstance()
{
	HANDLE	hMutex = CreateMutex(NULL, TRUE, TEXT("_QDL_ONEINSTANCE_MUTEX"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ExitProcess(0);
}


unsigned int WINAPI GetFSEventNotify(void *pvParam)
{
	DWORD	dwBytesRtn = 0;
	TCHAR		szEventInfo[MSGINFO_LENGTH] = TEXT("");

	while (TRUE)
	{
		WaitForSingleObject(QDLGlobalInfo.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_FILESCAN], INFINITE);
#ifdef	_DEBUG
		OutputDebugString(TEXT("RS:WaitForSingleObject"));
#endif
		if (DeviceIoControl(QDLGlobalInfo.hDev, IOCTL_QD_GETFILESCANINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDLGlobalInfo.hDlg, IDC_LIST_INFO);
			HWND		hWndInfo = GetDlgItem(QDLGlobalInfo.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;

			EnterCriticalSection(&QDLGlobalInfo.CS);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = QDLGlobalInfo.iCurItem;
			QDLGlobalInfo.iCurItem++;
			lvItem.pszText= szItem;

			
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 0"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("读取文件/目录"));
			ListView_SetItem(hWndLv, &lvItem);
			iPos++;

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 1"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);
#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 2"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 3;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDLGlobalInfo.iCurItem);
			Static_SetText(hWndInfo, szItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			LeaveCriticalSection(&QDLGlobalInfo.CS);
		}
	}
	return(0);
}

unsigned int WINAPI GetRSEventNotify(void *pvParam)
{
	DWORD	dwBytesRtn = 0;
	TCHAR		szEventInfo[MSGINFO_LENGTH] = TEXT("");

	while (TRUE)
	{
		WaitForSingleObject(QDLGlobalInfo.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_REGSCAN], INFINITE);
#ifdef	_DEBUG
		OutputDebugString(TEXT("RS:WaitForSingleObject"));
#endif
		if (DeviceIoControl(QDLGlobalInfo.hDev, IOCTL_QD_GETREGSCANINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDLGlobalInfo.hDlg, IDC_LIST_INFO);
			HWND		hWndInfo = GetDlgItem(QDLGlobalInfo.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;

			EnterCriticalSection(&QDLGlobalInfo.CS);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = QDLGlobalInfo.iCurItem;
			InterlockedIncrement(&QDLGlobalInfo.iCurItem);
			lvItem.pszText= szItem;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 0"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("访问注册表"));
			ListView_SetItem(hWndLv, &lvItem);
			iPos++;

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 1"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);
#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 2"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 3;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("未拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDLGlobalInfo.iCurItem);
			Static_SetText(hWndInfo, szItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			LeaveCriticalSection(&QDLGlobalInfo.CS);
		}
	}
	return(0);
}

unsigned int WINAPI GetOPEventNotify(void *pvParam)
{
	DWORD	dwBytesRtn = 0;
	TCHAR		szEventInfo[MSGINFO_LENGTH] = TEXT("");

	while (TRUE)
	{
		WaitForSingleObject(QDLGlobalInfo.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_OPPROC], INFINITE);
#ifdef	_DEBUG
		OutputDebugString(TEXT("OP:WaitForSingleObject"));
#endif
		if (DeviceIoControl(QDLGlobalInfo.hDev, IOCTL_QD_GETOPPROCINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDLGlobalInfo.hDlg, IDC_LIST_INFO);
			HWND		hWndInfo = GetDlgItem(QDLGlobalInfo.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;
			EnterCriticalSection(&QDLGlobalInfo.CS);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = QDLGlobalInfo.iCurItem;
			InterlockedIncrement(&QDLGlobalInfo.iCurItem);
			lvItem.pszText= szItem;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 0"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("访问进程"));
			ListView_SetItem(hWndLv, &lvItem);
			iPos++;

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 1"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);
#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 2"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 3;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDLGlobalInfo.iCurItem);
			Static_SetText(hWndInfo, szItem);
			
			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			LeaveCriticalSection(&QDLGlobalInfo.CS);
		}
	}
	return(0);
}

unsigned int WINAPI GetCPEventNotify(void *pvParam)
{
	DWORD	dwBytesRtn = 0;
	TCHAR		szEventInfo[MSGINFO_LENGTH] = TEXT("");

	while (TRUE)
	{
		WaitForSingleObject(QDLGlobalInfo.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_CRTPROC], INFINITE);
#ifdef	_DEBUG
		OutputDebugString(TEXT("CP:WaitForSingleObject"));
#endif
		if (DeviceIoControl(QDLGlobalInfo.hDev, IOCTL_QD_GETCRTPROCINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDLGlobalInfo.hDlg, IDC_LIST_INFO);
			HWND		hWndInfo = GetDlgItem(QDLGlobalInfo.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;
			EnterCriticalSection(&QDLGlobalInfo.CS);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = QDLGlobalInfo.iCurItem;
			InterlockedIncrement(&QDLGlobalInfo.iCurItem);
			lvItem.pszText= szItem;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 0"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("创建进程"));
			ListView_SetItem(hWndLv, &lvItem);
			iPos++;

#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 1"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);
#ifdef _DEBUG
			OutputDebugString(TEXT("iSubItem = 2"));
			OutputDebugString(szItem);
#endif

			lvItem.iSubItem = 3;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDLGlobalInfo.iCurItem);
			Static_SetText(hWndInfo, szItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);
			LeaveCriticalSection(&QDLGlobalInfo.CS);
		}
	}
	return(0);
}

void InitSystem(HWND hDlg)
{
	InitializeCriticalSection(&QDLGlobalInfo.CS);

	HANDLE	hSyncEvent[QD_SYNC_EVENTNUMS] = {NULL};

	for (int i = 0; i < QD_SYNC_EVENTNUMS; i++)
	{
		hSyncEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		QDLGlobalInfo.QDLEvt.hSyncEvent[i] = hSyncEvent[i];
	}

	HANDLE	hDev = CreateFile(TEXT("\\\\.\\QQDetective"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDev != INVALID_HANDLE_VALUE)
	{
		DWORD	dwBytesRtn;
		if (DeviceIoControl(hDev, IOCTL_QD_INITSYNCEVENT, &hSyncEvent, sizeof (hSyncEvent), NULL, 0, &dwBytesRtn, NULL))
		{
			//Create thread here
			_beginthreadex(NULL, 0, GetFSEventNotify, NULL, 0, NULL);
			_beginthreadex(NULL, 0, GetRSEventNotify, NULL, 0, NULL);
			_beginthreadex(NULL, 0, GetOPEventNotify, NULL, 0, NULL);
			_beginthreadex(NULL, 0, GetCPEventNotify, NULL,0, NULL);
		}
		QDLGlobalInfo.hDev = hDev;
	}
}

BOOL Cls_OnInitDlg(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	HWND			hWndLv = GetDlgItem(hDlg, IDC_LIST_INFO);
	LVCOLUMN	lvColumn = {0};
	LVITEM			lvItem = {0};
	TCHAR			szItemInfo[256] = TEXT("");

	SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(QDLGlobalInfo.hInstExe, MAKEINTRESOURCE(IDI_APPICON)));
	ListView_SetExtendedListViewStyle(hWndLv, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER);

	lvColumn.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
	lvColumn.pszText = szItemInfo;

	LoadString(QDLGlobalInfo.hInstExe, IDS_LVC_EVENTSOURCE, szItemInfo, 256);
	lvColumn.cx = 70;
	ListView_InsertColumn(hWndLv, 0, &lvColumn);

	LoadString(QDLGlobalInfo.hInstExe, IDS_LVC_EVENTACTION, szItemInfo, 256);
	lvColumn.cx = 90;
	ListView_InsertColumn(hWndLv, 1, &lvColumn);

	LoadString(QDLGlobalInfo.hInstExe, IDS_LVC_EVENTOBJECT, szItemInfo, 256);
	lvColumn.cx = 420;
	ListView_InsertColumn(hWndLv, 2, &lvColumn);

	LoadString(QDLGlobalInfo.hInstExe, IDS_LVC_EVENTSTATE, szItemInfo, 256);
	lvColumn.cx = 80;
	lvColumn.fmt = LVCFMT_RIGHT;
	ListView_InsertColumn(hWndLv, 3, &lvColumn);

	ListView_SetOutlineColor(hWndLv, RGB(0, 0, 0));

	QDLGlobalInfo.hDlg = hDlg;
	InitSystem(hDlg);
	return(FALSE);
}

void Cls_OnCommand(HWND hDlg, int id, HWND hWndCtrl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
		EndDialog(hDlg, 0);
		break;
	case IDC_BTN_CLRLIST:
		{
			HWND		hWndLv = GetDlgItem(hDlg, IDC_LIST_INFO);
			HWND		hWndInfo = GetDlgItem(hDlg, IDC_STATIC_INFO);

			ListView_DeleteAllItems(hWndLv);
			InterlockedExchange(&QDLGlobalInfo.iCurItem, 0);
			Static_SetText(hWndInfo, TEXT("当前共有0条事件"));
		}
		break;
	}
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, Cls_OnInitDlg);
		HANDLE_MSG(hDlg, WM_COMMAND, Cls_OnCommand);
	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_CLICK:
			case	NM_RETURN:
				{
					PNMLINK		pLink = (PNMLINK)lParam;
					LITEM			LinkItem = pLink->item;

					if (((LPNMHDR)lParam)->hwndFrom == GetDlgItem(hDlg, IDC_SYSLINK_NEWINFO))
					{
						ShellExecute(NULL, TEXT("OPEN"), LinkItem.szUrl, NULL, NULL, SW_SHOW);
					}
				}
				break;
			}
		}
		break;
	}
	return(0);
}

int WINAPI _tWinMain(HINSTANCE hInstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	OneInstance();
	QDLGlobalInfo.hInstExe = hInstExe;
	DialogBox(hInstExe, MAKEINTRESOURCE(IDD_DLG_APPMAIN), NULL, DlgProc);
	DeleteCriticalSection(&QDLGlobalInfo.CS);
	CloseHandle(QDLGlobalInfo.hDev);
	return(0);
}