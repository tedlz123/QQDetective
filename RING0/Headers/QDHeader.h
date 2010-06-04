/*
*	Instruction:	QQDetective Main Header
*	Date:			2010/5/16
*	Version:		1.0
*	Author:			Kernone
*	Blog:			http://hi.baidu.com/kernone
*	Copyright(c)	Kernone Soft 2010
*
*/

#include <ntifs.h>
#include <Ntstrsafe.h>
#include <ntddk.h>
#include "QDDataStruct.h"
#include "QDDeclFuncs.h"

//
//IO Control Code number, these indicate control
//driver hooks or not hooks the SSDT
//
#define	IOFUNC_FILE_START	0x805
#define	IOFUNC_FILE_STOP	0x806
#define	IOFUNC_REG_START	0x810
#define	IOFUNC_REG_STOP		0x811
#define	IOFUNC_CRTP_START	0x815
#define	IOFUNC_CRTP_STOP	0x816
#define	IOFUNC_OPP_START	0x820
#define	IOFUNC_OPP_STOP		0x821

//
//IO Control Code, control driver hook or not hook
//the System Services Descriptor Table
//
#define	IOCTL_QD_FILEPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_FILE_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_QD_FILEPROTECTION_STOP		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_FILE_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_REGPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_REG_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_REGPROTECTION_STOP			CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_REG_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_CRTPROCPROTECTION_START	CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_CRTP_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_CRTPROCPROTECTION_STOP		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_CRTP_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_OPPROCPROTECTION_START		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_OPP_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_OPPROCPROTECTION_STOP		CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_OPP_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
//IO Control Code number, these indicate transfer
//informations between kernel mode and user mode
//
#define IOFUNC_GET_FILESCANINFO	0x901
#define	IOFUNC_GET_REGSCANINFO	0x906
#define	IOFUNC_GET_OPPROCINFO	0x911
#define	IOFUNC_GET_CRTPROCINFO	0x916
#define	IOFUNC_INIT_SYNCEVENT	0x951

//
//Io Control Code, these IO code indicates transfer
//informations between kernel mode and user mode
//
#define	IOCTL_QD_INITSYNCEVENT				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_INIT_SYNCEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETFILESCANINFO			CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_FILESCANINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETREGSCANINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_REGSCANINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETOPPROCINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_OPPROCINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_QD_GETCRTPROCINFO				CTL_CODE(FILE_DEVICE_UNKNOWN, IOFUNC_GET_CRTPROCINFO, METHOD_BUFFERED, FILE_ANY_ACCESS)





//
//Defining device name and symbolic name
//
#define	QDDEVICENAME		L"\\Device\\KSQQDetective"
#define	QDSYMBOLICLINK		L"\\DosDevices\\QQDetective"


extern	ZWOPENPROCESS		ZwOpenProcessReal;
extern	ZWCREATESECTION		ZwCreateSectionReal;
extern	ZWOPENFILE			ZwOpenFileReal;
extern	ZWOPENKEY			ZwOpenKeyReal;



extern 	ULONG				ulProcNameOffset;
extern 	QDSYSTEMGLOBALINFO	QDSystemGlobalInfo;


extern	PMDL				g_pMdl;
extern	PULONG				g_pSSDTMapped;

extern	PSTR				pchProcName[INTERCEPTION_PROCNUMS];
extern	PWSTR				pwsDirectory[ALLOWACCESS_DIRS];
extern	PWSTR				pwsRegPath[KEYACCESS_REG];



extern	PWSTR				pwsCrtProcessEvent;
extern	PWSTR				pwsOpProcessEvent;
extern	PWSTR				pwsFileScanEvent;
extern	PWSTR				pwsRegScanEvent;


extern	PKEVENT				pKEvtSync[QD_SYNC_EVENTNUMS];

extern	KEVENT				KEvtOpenFile;
extern	KEVENT				KEvtOpenKey;