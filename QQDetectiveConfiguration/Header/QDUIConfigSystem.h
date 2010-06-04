#include "QDSystem.h"


WNDPROC CheckWndProc[INTERCEPTION_FUNCNUM] = {NULL};

INT_PTR	CALLBACK DlgProc_SysCfg(
	HWND hWndPage,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	);

void SysCfgProc_OnCommand(
	HWND hWndPage,
	int id,
	HWND hWndCtrl,
	UINT uiNotify
	);

BOOL SysCfgProc_OnInitPage(
	HWND hWndPage,
	HWND hWndFocus,
	LPARAM lParam
	);

void SysCfgProc_OnVScroll(
	HWND hWndPage,
	HWND hWndCtrl,
	UINT uiNotify,
	int iPos
	);

void VerifyRank(
	HWND hWndTBRank
	);

BOOL UpdateSysCfgRegistry(
	);
