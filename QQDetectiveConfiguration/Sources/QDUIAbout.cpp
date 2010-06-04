#include "..\\Header\\QDUIAbout.h"

BOOL About_OnInitDialog(HWND hWndPage, HWND hWndFocus, LPARAM lParam)
{

	return(FALSE);
}

INT_PTR CALLBACK DlgProc_About(HWND hWndPage, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWndPage, WM_INITDIALOG, About_OnInitDialog);
	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_CLICK:
			case NM_RETURN:
				{
					PNMLINK		pLink = (PNMLINK)lParam;
					LITEM			lItem = pLink->item;

					if (((LPNMHDR)lParam)->hwndFrom == GetDlgItem(hWndPage, IDC_SYSLINK_ABOUT))
					{
						ShellExecute(NULL, TEXT("OPEN"), lItem.szUrl, NULL, NULL, SW_SHOWNORMAL);
					}
				}
				break;
			}
		}
		break;
	}
	return(0);
}