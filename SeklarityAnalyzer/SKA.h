//Seklarity.com

#pragma once

#include "globals.h"
#include "modules/kuhl_m_crypto.h"
#include "modules/sekurlsa/kuhl_m_sekurlsa.h"
#include "modules/kerberos/kuhl_m_kerberos.h"
#include "modules/kuhl_m_process.h"
#include "modules/kuhl_m_service.h"
#include "modules/kuhl_m_privilege.h"
#include "modules/kuhl_m_lsadump.h"
#include "modules/kuhl_m_ts.h"
#include "modules/kuhl_m_event.h"
#include "modules/kuhl_m_misc.h"
#include "modules/kuhl_m_token.h"
#include "modules/kuhl_m_vault.h"
#ifdef NET_MODULE
#include "modules/kuhl_m_net.h"
#endif
#include "modules/kuhl_m_kernel.h"
#include "modules/kuhl_m_busylight.h"
#include "modules/kuhl_m_sysenvvalue.h"
#include "modules/kuhl_m_sid.h"

#include <stdio.h>  
#include <malloc.h>  
#include <stdlib.h>  
#include <io.h>
#include <fcntl.h>

extern VOID WINAPI RtlGetNtVersionNumbers(LPDWORD pMajor, LPDWORD pMinor, LPDWORD pBuild);

void doSomething();

int wmain(int argc, wchar_t * argv[]);

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);

NTSTATUS sekcee_initOrClean();

NTSTATUS sekcee_doLocal(wchar_t * input);
NTSTATUS sekcee_dispatchCommand(wchar_t * input);

#ifdef _WINDLL
__declspec(dllexport) wchar_t * powershell_reflective_sekcee(LPCWSTR input);
#endif

typedef struct SampleStruct
{
	char* id;
	char* username;
	int val;
	struct SampleStruct *next;
} SampleStruct;



