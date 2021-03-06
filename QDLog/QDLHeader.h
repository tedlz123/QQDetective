#include <Windows.h>
#include <WindowsX.h>
#include <process.h>
#include <WinIoCtl.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"

#if defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define	QD_SYNC_EVENTNUMS							4
#define	QD_SYNC_EVENTNAME_FILESCAN			0
#define	QD_SYNC_EVENTNAME_REGSCAN			1
#define	QD_SYNC_EVENTNAME_OPPROC			2
#define	QD_SYNC_EVENTNAME_CRTPROC			3

//
//IO Control Code number, these indicate transfer
//informations between kernel mode and user mode
//
#define IOFUNC_GET_FILESCANINFO	0x901
#define	IOFUNC_GET_REGSCANINFO	0x906
#define	IOFUNC_GET_OPPROCINFO	0x911
#define	IOFUNC_GET_CRTPROCINFO	0x916
#define	IOFUNC_INIT_SYNCEVENT		0x951

//
//Io Control Code, these IO code indicates transfer
//informations between kernel mode and user mode
//
#define	IOCTL_QD_INITSYNCEVENT					CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_INIT_SYNCEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETFILESCANINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_FILESCANINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETREGSCANINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_REGSCANINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETOPPROCINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_OPPROCINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETCRTPROCINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_CRTPROCINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define	MSGINFO_LENGTH	320

typedef struct _QDL_GLOBAL_EVENT
{
	HANDLE		hSyncEvent[QD_SYNC_EVENTNUMS];
}QDLGE, *PQDLGE;
typedef struct _QDL_GLOBAL_INFO
{
	QDLGE						QDLEvt;
	HINSTANCE				hInstExe;
	HANDLE					hDev;
	HWND						hDlg;
	long							iCurItem;
	CRITICAL_SECTION	CS;
}QDLGLOBALINFO, *PQDLGLOBALINFO;