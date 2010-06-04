#include "..\\Header\\QDSystem.h"

unsigned int WINAPI InitSystemThread(void *pvParam)
{
	HANDLE hInitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, QD_EVENTNAME_INITSYSTEM);
	WaitForSingleObject(hInitEvent, INFINITE);
	Static_SetText(GetDlgItem((HWND)pvParam, IDC_STATIC_INITINFO), TEXT("初始化完成！"));
	CloseHandle(hInitEvent);
	Sleep(200);
	EndDialog((HWND)pvParam, 0);
	return(0);
}

BOOL InitSystem_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	HWND hWndProgress = GetDlgItem(hDlg, IDC_PROGRESS_STATUS);

	SetWindowLongPtr(hWndProgress, GWL_STYLE, GetWindowLongPtr(hWndProgress, GWL_STYLE) | PBS_MARQUEE);
	SendMessage(hWndProgress, PBM_SETMARQUEE, TRUE, 0);
	ShowWindow(hDlg, SW_SHOWNORMAL);
	//SendMessage(hDlg, WM_INITSYSTEM, 0, 0);
	_beginthreadex(NULL, 0, InitSystemThread, (void *)hDlg, 0, NULL);
	return(FALSE);
}

INT_PTR CALLBACK DlgProc_InitSystem(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, InitSystem_OnInitDialog);
	}
	return(0);
}