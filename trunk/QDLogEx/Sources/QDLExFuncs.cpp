#include "..\\Headers\\QDLogEx.h"

void OneInstance()
{
	HANDLE	hMutex = CreateMutex(NULL, TRUE, TEXT("_QDL_ONEINSTANCE_MUTEX"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ExitProcess(0);
}

void InitListView(HWND hDlg)
{
	HWND			hList[ID_LIST_ALL] = {0};
	LVCOLUMN	lvCol = {0};

	lvCol.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;

	for (int i = 0; i < ID_LIST_ALL; i++)
	{
		hList[i] = GetDlgItem(hDlg, IDC_LIST_FS + i);

		ListView_SetExtendedListViewStyle(hList[i], LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

		lvCol.cx = 80;
		lvCol.pszText = TEXT("事件源");
		ListView_InsertColumn(hList[i], 0, &lvCol);

		lvCol.cx = 470;
		lvCol.pszText = TEXT("事件对象");
		ListView_InsertColumn(hList[i], 1, &lvCol);

		lvCol.cx = 100;
		lvCol.pszText = TEXT("事件状态");
		ListView_InsertColumn(hList[i], 2, &lvCol);
	}
}

void ShowSpecifyList(HWND hDlg, UINT uSpecifyID)
{
	HWND	hList[ID_LIST_ALL] = {0};

	for (int i = 0; i < ID_LIST_ALL; i++)
	{
		hList[i] = GetDlgItem(hDlg, IDC_LIST_FS + i);
		ShowWindow(hList[i], SW_HIDE);
	}

	for (int i = 0; i < ID_LIST_ALL; i++)
	{
		if (IDC_LIST_FS + i == uSpecifyID)
		{
			ShowWindow(hList[i], SW_SHOW);
			return;
		}
	}
}


unsigned int WINAPI GetFSEventNotify(void *pvParam)
{
	DWORD	dwBytesRtn = 0;
	TCHAR		szEventInfo[MSGINFO_LENGTH] = TEXT("");

	while (TRUE)
	{
		WaitForSingleObject(QDL.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_FILESCAN], INFINITE);
		EnterCriticalSection(&QDL.CSFS);
		if (DeviceIoControl(QDL.hDev, IOCTL_QD_GETFILESCANINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDL.hDlg, IDC_LIST_FS);
			HWND		hWndInfo = GetDlgItem(QDL.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;

			lvItem.mask = LVIF_TEXT;
			lvItem.pszText = szItem;
			lvItem.iItem = QDL.ulFS++;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

			//lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			iPos++;

			lvItem.iSubItem = 1;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);	

			OutputDebugString(szItem);

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			InterlockedIncrement(&QDL.ulTotal);
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDL.ulTotal);
			Static_SetText(hWndInfo, szItem);

			LeaveCriticalSection(&QDL.CSFS);
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
		WaitForSingleObject(QDL.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_REGSCAN], INFINITE);

		if (DeviceIoControl(QDL.hDev, IOCTL_QD_GETREGSCANINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDL.hDlg, IDC_LIST_RS);
			HWND		hWndInfo = GetDlgItem(QDL.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;


			lvItem.mask = LVIF_TEXT;
			lvItem.pszText= szItem;
			lvItem.iItem = QDL.ulRS++;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

			//lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			iPos++;

			lvItem.iSubItem = 1;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);

			OutputDebugString(szItem);

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("未拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			InterlockedIncrement(&QDL.ulTotal);
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDL.ulTotal);
			Static_SetText(hWndInfo, szItem);
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
		WaitForSingleObject(QDL.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_OPPROC], INFINITE);

		if (DeviceIoControl(QDL.hDev, IOCTL_QD_GETOPPROCINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDL.hDlg, IDC_LIST_OP);
			HWND		hWndInfo = GetDlgItem(QDL.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;

			lvItem.mask = LVIF_TEXT;
			lvItem.pszText= szItem;
			lvItem.iItem = QDL.ulOP++;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

			//lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			iPos++;

			lvItem.iSubItem = 1;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			InterlockedIncrement(&QDL.ulTotal);
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDL.ulTotal);
			Static_SetText(hWndInfo, szItem);
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
		WaitForSingleObject(QDL.QDLEvt.hSyncEvent[QD_SYNC_EVENTNAME_CRTPROC], INFINITE);

		if (DeviceIoControl(QDL.hDev, IOCTL_QD_GETCRTPROCINFO, NULL, 0, szEventInfo, sizeof (szEventInfo), &dwBytesRtn, NULL))
		{
			HWND		hWndLv = GetDlgItem(QDL.hDlg, IDC_LIST_CP);
			HWND		hWndInfo = GetDlgItem(QDL.hDlg, IDC_STATIC_INFO);
			LVITEM		lvItem = {0};
			TCHAR		szItem[MSGINFO_LENGTH] = TEXT("");
			INT			i = 0, iPos = 0;

			lvItem.mask = LVIF_TEXT;
			lvItem.pszText= szItem;
			lvItem.iItem = QDL.ulCP++;

			lvItem.iSubItem = 0;	
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_InsertItem(hWndLv, &lvItem);
			//Skip current ';'
			iPos++;
			ZeroMemory(szItem, sizeof (szItem));

			//lvItem.iSubItem = 1;
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			iPos++;

			lvItem.iSubItem = 1;
			ZeroMemory(szItem, sizeof (szItem));
			for (i = 0; szEventInfo[iPos] != TEXT(';'); i++, iPos++)
			{
				szItem[i] = szEventInfo[iPos];
			}
			ListView_SetItem(hWndLv, &lvItem);

			lvItem.iSubItem = 2;
			ZeroMemory(szItem, sizeof (szItem));
			StringCchCopy(szItem, MSGINFO_LENGTH, TEXT("已拦截"));
			ListView_SetItem(hWndLv, &lvItem);

			//StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDL.iCurItem);
			//Static_SetText(hWndInfo, szItem);

			SendMessage(hWndLv, WM_VSCROLL, SB_LINEDOWN, 0);

			InterlockedIncrement(&QDL.ulTotal);
			StringCchPrintf(szItem, MSGINFO_LENGTH, TEXT("当前共有%ld条事件"), QDL.ulTotal);
			Static_SetText(hWndInfo, szItem);
		}
	}
	return(0);
}


void InitSystem(HWND hDlg)
{
	HANDLE	hSyncEvent[QD_SYNC_EVENTNUMS] = {NULL};

	for (int i = 0; i < QD_SYNC_EVENTNUMS; i++)
	{
		hSyncEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		QDL.QDLEvt.hSyncEvent[i] = hSyncEvent[i];
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
		QDL.hDev = hDev;
	}
}