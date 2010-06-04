#include <Windows.h>
#include <WindowsX.h>
#include <process.h>
#include <CommCtrl.h>
#include <WinIoCtl.h>
#include <PrSht.h>
#include <ShlObj.h>
#include <tchar.h>
#include <strsafe.h>
#include "QDDataStruct.h"
#include "..\resource.h"

//
//Defining Build Environment
//
//#define	_WIN7_ENV

#ifndef _WIN7_ENV
#define		_WIN_XP							0x501
#else 
#define		_WIN_7							0x601
#endif

//Defining some macros
#define		MAXCHARACTERNUM		12
#define		MAX_INSTRUCTION			128
#define		QDPAGES_NUMS				3
#define		QDPAGES_SYSCFG			0
#define		QDPAGES_SYSMAN			1			//Release version,  modify it to 2
#define		QDPAGES_ABOUT			2

#define		STRING_QDDEVNAME		TEXT("\\\\.\\QQDetective")
#define		STRING_ONEINSTANCE	TEXT("_KernoneSoft_QQDetective_Mutex")
//Other statements
#if defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define	IOFUNC_FILE_START	0x805
#define	IOFUNC_FILE_STOP		0x806
#define	IOFUNC_REG_START	0x810
#define	IOFUNC_REG_STOP		0x811
#define	IOFUNC_CRTP_START	0x815
#define	IOFUNC_CRTP_STOP	0x816
#define	IOFUNC_OPP_START	0x820
#define	IOFUNC_OPP_STOP		0x821

#define	IOCTL_QD_FILEPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_FILE_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_QD_FILEPROTECTION_STOP			CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_FILE_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_REGPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_REG_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_REGPROTECTION_STOP			CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_REG_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_CRTPROCPROTECTION_START	CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_CRTP_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_CRTPROCPROTECTION_STOP		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_CRTP_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_OPPROCPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_OPP_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_OPPROCPROTECTION_STOP		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_OPP_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define QD_EVENTNAME_INITSYSTEM					TEXT("QD_EVENT_INITSYSTEM")

BOOL InitGetRegCfgInfo(
	);

BOOL UpdateSysCfgRegistry(
	);

BOOL InitSetDefRegCfgInfo(
	);

void InitializeSystem(
	);

void ControlDriverFunc(
	HANDLE	hQDDev
	);

INT_PTR CALLBACK DlgProc_ManSys(
	HWND hWndPage,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	);

INT_PTR CALLBACK DlgProc_InitSystem(
	HWND hDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam
	);

INT_PTR CALLBACK DlgProc_About(
	HWND hWndPage, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam
	);

void UpdateSysManRegistry(
	);

void ControlDriverFunc(
	HANDLE	hQDDev
	);

BOOL InstallDriverModule(
	BOOL	bShowMsg
	);

BOOL UnInstallDriverModule(
	BOOL bShowMsg
	);

extern QDSYSGLOBALINFO	QDSystemGlobalInfo;
