/*
	sekcee_extractor
	
	
*/
#pragma once
#include <ntstatus.h>
#define WIN32_NO_STATUS
#define SECURITY_WIN32
#define CINTERFACE
#define COBJMACROS
#include <windows.h>
#include <sspi.h>
#include <sddl.h>
#include <wincred.h>
#include <ntsecapi.h>
#include <ntsecpkg.h>
#include <stdio.h>
#include <wchar.h>
#include <stdio.h>  
#include <malloc.h>  
#include <stdlib.h>  
#include "../modules/kull_m_output.h"
//#define KERBEROS_TOOLS
//#define LSASS_DECRYPT
#define NET_MODULE
#define SQLITE3_OMIT
#ifdef _M_X64
	#define SEKCEE_ARCH L"x64"
#else ifdef _M_IX86
	#define SEKCEE_ARCH L"x86"
#endif

#define SEKCEE				L"sekcee"
#define SEKCEE_VERSION		L"1.0"
#define SEKCEE_CODENAME		L"sekcee v1"
#define SEKCEE_FULL			SEKCEE L" " SEKCEE_VERSION L" (" SEKCEE_ARCH L") built on " TEXT(__DATE__) L" " TEXT(__TIME__)
#define SEKCEE_SECOND			L"\"" SEKCEE_CODENAME L"\""
#define SEKCEE_SPECIAL		L"                                "
#define SEKCEE_DEFAULT_LOG	SEKCEE L".log"
#define SEKCEE_DRIVER			L"sekCDriver"
#define SEKCEE_KERBEROS_EXT	L"kerbex"

#ifdef _WINDLL
	#define SEKCEE_AUTO_COMMAND_START		0
	#define SEKCEE_AUTO_COMMAND_STRING	L"powershell"
#else
	#define SEKCEE_AUTO_COMMAND_START		1
	#define SEKCEE_AUTO_COMMAND_STRING	L"commandline"
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef PRINT_ERROR
#define PRINT_ERROR(...) (sekcee_stdout(L"ERROR " TEXT(__FUNCTION__) L" ; " __VA_ARGS__))
#endif

#ifndef PRINT_ERROR_AUTO
#define PRINT_ERROR_AUTO(func) (sekcee_stdout(L"ERROR " TEXT(__FUNCTION__) L" ; " func L" (0x%08x)\n", GetLastError()))
#endif

#ifndef W00T
#define W00T(...) (sekcee_stdout(TEXT(__FUNCTION__) L" w00t! ; " __VA_ARGS__))
#endif

DWORD SEKCEE_NT_MAJOR_VERSION, SEKCEE_NT_MINOR_VERSION, SEKCEE_NT_BUILD_NUMBER;

#ifndef MS_ENH_RSA_AES_PROV_XP
#define MS_ENH_RSA_AES_PROV_XP	L"Microsoft Enhanced RSA and AES Cryptographic Provider (Prototype)"
#endif

#define RtlEqualGuid(L1, L2) (RtlEqualMemory(L1, L2, sizeof(GUID)))

#define SIZE_ALIGN(size, alignment)	(size + ((size % alignment) ? (alignment - (size % alignment)) : 0))

#define LM_NTLM_HASH_LENGTH	16

#define KULL_M_WIN_BUILD_XP		2600
#define KULL_M_WIN_BUILD_2K3	3790
#define KULL_M_WIN_BUILD_VISTA	6000
#define KULL_M_WIN_BUILD_7		7600
#define KULL_M_WIN_BUILD_8		9200
#define KULL_M_WIN_BUILD_BLUE	9600
#define KULL_M_WIN_BUILD_10_1507		10240
#define KULL_M_WIN_BUILD_10_1511		10586
#define KULL_M_WIN_BUILD_10_1607		14393
#define KULL_M_WIN_BUILD_10_1707		15063

#define KULL_M_WIN_MIN_BUILD_XP		2500
#define KULL_M_WIN_MIN_BUILD_2K3	3000
#define KULL_M_WIN_MIN_BUILD_VISTA	5000
#define KULL_M_WIN_MIN_BUILD_7		7000
#define KULL_M_WIN_MIN_BUILD_8		8000
#define KULL_M_WIN_MIN_BUILD_BLUE	9400
#define KULL_M_WIN_MIN_BUILD_10		9800

static int retCount;
typedef struct LogonStruct
{
	wchar_t*	authid;
	wchar_t*	session;
	wchar_t*	username;
	wchar_t*	domain;
	wchar_t*	logonserv;
	wchar_t*	logondate;
	//wchar_t*	logontime;
	wchar_t*	sid;
	BOOL	ntlmcred;
	BOOL	sha1cred;
	BOOL	netpwdcred;
	int val;
	int tp;
	struct LogonStruct *next;
} LogonStruct;

struct LogonStruct* create_logon_list(int val);
struct LogonStruct* add_to_logon_list(int val, BOOL add_to_end);

struct LogonStruct *head_logon;
struct LogonStruct *curr_logon;
struct LogonStruct *ptr_logon;
