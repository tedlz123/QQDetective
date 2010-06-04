#include "..\\Headers\\QDLogEx.h"

QDLGLOBALINFO	QDL;

void Cls_OnCommand(HWND hDlg, int id, HWND hWndCtrl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
		EndDialog(hDlg, 0);
		break;
	case IDC_BTN_CLEAR:
		{

		}
		break;
	}
}

BOOL Cls_OnInitDlg(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	HWND		hWndTab = GetDlgItem(hDlg, IDC_TAB_ITEM);
	TCITEM		tbItem = {0};
	TCHAR		szTbItem[64] = TEXT("");

	QDL.hDlg = hDlg;

	SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(QDL.hInstExe, MAKEINTRESOURCE(IDI_APPICON)));

	tbItem.mask = TCIF_TEXT;
	tbItem.pszText = szTbItem;

	for (int i = 0; i < 4; i++)
	{
		LoadString(QDL.hInstExe, IDS_TAB_FS + i, szTbItem, _countof(szTbItem));
		TabCtrl_InsertItem(hWndTab, i, &tbItem);
	}

	InitListView(hDlg);
	ShowSpecifyList(hDlg, IDC_LIST_FS);

	InitSystem(hDlg);
	return(FALSE);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, Cls_OnInitDlg);
		HANDLE_MSG(hDlg, WM_COMMAND, Cls_OnCommand);
	case WM_ACTIVATE:
		UpdateWindow(hDlg);
		break;
	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case TCN_SELCHANGE:
				{
					HWND	hWndTab = GetDlgItem(hDlg, IDC_TAB_ITEM);
					HWND	hWndLv;
					int iPage = TabCtrl_GetCurSel(hWndTab);

					hWndLv = GetDlgItem(hDlg, IDC_LIST_FS + iPage);
					SendMessage(hWndLv, WM_VSCROLL, SB_BOTTOM, 0);
					ShowSpecifyList(hDlg, IDC_LIST_FS + iPage);
					InvalidateRect(hWndLv, NULL, TRUE);
				}
				break;
			case NM_CLICK:
			case NM_RETURN:
				{
					PNMLINK		pLink = (PNMLINK)lParam;
					LITEM			LinkItem = pLink->item;

					if (((LPNMHDR)lParam)->hwndFrom == GetDlgItem(hDlg, IDC_SYSLINK_URL))
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

int WINAPI _tWinMain(HINSTANCE hInstExe, HINSTANCE, PTSTR pszCmdLine, int nCmdShow)
{
	OneInstance();
	QDL.hInstExe = hInstExe;
	InitializeCriticalSection(&QDL.CSFS);
	DialogBox(hInstExe, MAKEINTRESOURCE(IDD_DLG_MAIN), NULL, DlgProc);
	return(0);
}