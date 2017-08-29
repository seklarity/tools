/*
	sekcee_extractor
	
	
*/
#include "kull_m_string.h"

//BOOL kull_m_string_suspectUnicodeStringStructure(IN PUNICODE_STRING pUnicodeString)
//{
//	return (
//		pUnicodeString->Length &&
//		!((pUnicodeString->Length & 1) || (pUnicodeString->MaximumLength & 1)) &&
//		(pUnicodeString->Length < sizeof(wchar_t)*0xff) &&
//		(pUnicodeString->Length <= pUnicodeString->MaximumLength) &&
//		((pUnicodeString->MaximumLength - pUnicodeString->Length) < 4*sizeof(wchar_t)) &&
//		pUnicodeString->Buffer
//		);
//}

BOOL kull_m_string_suspectUnicodeString(IN PUNICODE_STRING pUnicodeString)
{
	int unicodeTestFlags = IS_TEXT_UNICODE_STATISTICS;
	return ((pUnicodeString->Length == sizeof(wchar_t)) && IsCharAlphaNumeric(pUnicodeString->Buffer[0])) || IsTextUnicode(pUnicodeString->Buffer, pUnicodeString->Length, &unicodeTestFlags);
}

void kull_m_string_printSuspectUnicodeString(PVOID data, DWORD size)
{
	UNICODE_STRING uString = {(USHORT) size, (USHORT) size, (LPWSTR) data};
	if(kull_m_string_suspectUnicodeString(&uString))
		sekcee_stdout(L"%wZ", &uString);
	else 
		kull_m_string_wprintf_hex(uString.Buffer, uString.Length, 1);
}

BOOL kull_m_string_getUnicodeString(IN PUNICODE_STRING string, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_HANDLE hOwn = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS aDestin = {NULL, &hOwn};
	KULL_M_MEMORY_ADDRESS aSource = {string->Buffer, source};
	
	string->Buffer = NULL;
	if(aSource.address && string->MaximumLength)
	{
		if(aDestin.address = LocalAlloc(LPTR, string->MaximumLength))
		{
			string->Buffer = (PWSTR) aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, string->MaximumLength);
		}
	}
	return status;
}

BOOL kull_m_string_getSid(IN PSID * pSid, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	BYTE nbAuth;
	DWORD sizeSid;
	KULL_M_MEMORY_HANDLE hOwn = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS aDestin = {&nbAuth, &hOwn};
	KULL_M_MEMORY_ADDRESS aSource = {(PBYTE) *pSid + 1, source};

	*pSid = NULL;
	if(kull_m_memory_copy(&aDestin, &aSource, sizeof(BYTE)))
	{
		aSource.address = (PBYTE) aSource.address - 1;
		sizeSid =  4 * nbAuth + 6 + 1 + 1;

		if(aDestin.address = LocalAlloc(LPTR, sizeSid))
		{
			*pSid = (PSID) aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, sizeSid);
		}
	}
	return status;
}

void kull_m_string_MakeRelativeOrAbsoluteString(PVOID BaseAddress, PLSA_UNICODE_STRING String, BOOL relative)
{
	if(String->Buffer)
		String->Buffer = (PWSTR) ((ULONG_PTR)(String->Buffer) + ((relative ? -1 : 1) * (ULONG_PTR)(BaseAddress)));
}

BOOL kull_m_string_copyUnicodeStringBuffer(PUNICODE_STRING pSource, PUNICODE_STRING pDestination)
{
	BOOL status = FALSE;
	if(pSource && pDestination && pSource->MaximumLength && pSource->Buffer)
	{
		*pDestination = *pSource;
		if(pDestination->Buffer = (PWSTR) LocalAlloc(LPTR, pSource->MaximumLength))
		{
			status = TRUE;
			RtlCopyMemory(pDestination->Buffer, pSource->Buffer, pSource->MaximumLength);
		}
	}
	return status;
}

void kull_m_string_freeUnicodeStringBuffer(PUNICODE_STRING pString)
{
	if(pString && pString->Buffer)
		pString->Buffer = (PWSTR) LocalFree(pString->Buffer);
}

wchar_t * kull_m_string_qad_ansi_to_unicode(const char * ansi)
{
	wchar_t * buffer = NULL;
	if(ansi)
		buffer = kull_m_string_qad_ansi_c_to_unicode(ansi, strlen(ansi));
	return buffer;
}

wchar_t * kull_m_string_qad_ansi_c_to_unicode(const char * ansi, SIZE_T szStr)
{
	wchar_t * buffer = NULL;
	SIZE_T i;

	if(ansi && szStr)
		if(buffer = (wchar_t *) LocalAlloc(LPTR, (szStr + 1) * sizeof(wchar_t)))
			for(i = 0; i < szStr; i++)
				buffer[i] = ansi[i];
	return buffer;
}

char * kull_m_string_unicode_to_ansi(const wchar_t * unicode)
{
	int needed;
	char * buffer = NULL;
	if(needed = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, unicode, -1, NULL, 0, NULL, NULL))
		if(buffer = (char *) LocalAlloc(LPTR, needed))
			if(needed != WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, unicode, -1, buffer, needed, NULL, NULL))
				buffer = (char *) LocalFree(buffer);
	return buffer;
}

BOOL kull_m_string_stringToHex(IN LPCWCHAR string, IN LPBYTE hex, IN DWORD size)
{
	DWORD i, j;
	BOOL result = (wcslen(string) == (size * 2));
	if(result)
	{
		for(i = 0; i < size; i++)
		{
			swscanf_s(&string[i*2], L"%02x", &j);
			hex[i] = (BYTE) j;
		}
	}
	return result;
}

BOOL kull_m_string_stringToHexBuffer(IN LPCWCHAR string, IN LPBYTE *hex, IN DWORD *size)
{
	BOOL result = FALSE;
	*size = (DWORD) wcslen(string);
	if(!(*size % 2))
	{
		*size >>= 1;
		if(*hex = (PBYTE) LocalAlloc(LPTR, *size))
		{
			result = kull_m_string_stringToHex(string, *hex, *size);
			if(!result)
			{
				*hex = (PBYTE) LocalFree(*hex);
				*size = 0;
			}
		}
	}
	return result;
}

PCWCHAR WPRINTF_TYPES[] =
{
	L"%02x",		// WPRINTF_HEX_SHORT
	L"%02x ",		// WPRINTF_HEX_SPACE
	L"0x%02x, ",	// WPRINTF_HEX_C
	L"\\x%02x",		// WPRINTF_HEX_PYTHON
};

void kull_m_string_wprintf_hex(LPCVOID lpData, DWORD cbData, DWORD flags)
{
	DWORD i, sep = flags >> 16;
	PCWCHAR pType = WPRINTF_TYPES[flags & 0x0000000f];

	if((flags & 0x0000000f) == 2)
		sekcee_stdout(L"\nBYTE data[] = {\n\t");

	for(i = 0; i < cbData; i++)
	{
		sekcee_stdout(pType, ((LPCBYTE) lpData)[i]);
		if(sep && !((i+1) % sep))
		{
			sekcee_stdout(L"\n");
			if((flags & 0x0000000f) == 2)
				sekcee_stdout(L"\t");
		}
	}
	if((flags & 0x0000000f) == 2)
		sekcee_stdout(L"\n};\n");
}

void kull_m_string_displayFileTime(IN PFILETIME pFileTime)
{
	SYSTEMTIME st;
	wchar_t buffer[0xff];
	if(pFileTime)
	{
		if(FileTimeToSystemTime(pFileTime, &st ))
		{
			if(GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, buffer, ARRAYSIZE(buffer)))
			{
				sekcee_stdout(L"%s ", buffer);
				if(GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, buffer, ARRAYSIZE(buffer)))
					sekcee_stdout(L"%s", buffer);
			}
		}
	}
}

void kull_m_string_displayLocalFileTime(IN PFILETIME pFileTime)
{
	FILETIME ft;
	if(pFileTime)
		if(FileTimeToLocalFileTime(pFileTime, &ft))
			kull_m_string_displayFileTime(&ft);
}

void kull_m_string_displayGUID(IN LPCGUID pGuid)
{
	UNICODE_STRING uString;
	if(NT_SUCCESS(RtlStringFromGUID(pGuid, &uString)))
	{
		sekcee_stdout(L"%wZ", &uString);
		RtlFreeUnicodeString(&uString);
	}
}

void kull_m_string_displaySID(IN PSID pSid)
{
	LPWSTR stringSid;
	if(ConvertSidToStringSid(pSid, &stringSid))
	{
		sekcee_stdout(L"%s", stringSid);
		LocalFree(stringSid);
	}
	else PRINT_ERROR_AUTO(L"ConvertSidToStringSid");
}
#ifndef SEKCEE_W2000_SUPPORT
PWSTR kull_m_string_getRandomGUID()
{
	UNICODE_STRING uString;
	GUID guid;
	PWSTR buffer = NULL;
	if(NT_SUCCESS(UuidCreate(&guid)))
	{
		if(NT_SUCCESS(RtlStringFromGUID(&guid, &uString)))
		{
			if(buffer = (PWSTR) LocalAlloc(LPTR, uString.MaximumLength))
				RtlCopyMemory(buffer, uString.Buffer, uString.MaximumLength);
			RtlFreeUnicodeString(&uString);
		}
	}
	return buffer;
}
#endif
void kull_m_string_ptr_replace(PVOID ptr, DWORD64 size)
{
	PVOID tempPtr = NULL;
	if(size)
		if(tempPtr = LocalAlloc(LPTR, (SIZE_T) size))
			RtlCopyMemory(tempPtr, *(PVOID *) ptr, (size_t) size);
	*(PVOID *) ptr = tempPtr;
}

BOOL kull_m_string_args_byName(const int argc, const wchar_t * argv[], const wchar_t * name, const wchar_t ** theArgs, const wchar_t * defaultValue)
{
	BOOL result = FALSE;
	const wchar_t *pArgName, *pSeparator;
	SIZE_T argLen, nameLen = wcslen(name);
	int i;
	for(i = 0; i < argc; i++)
	{
		if((wcslen(argv[i]) > 1) && ((argv[i][0] == L'/') || (argv[i][0] == L'-')))
		{
			pArgName = argv[i] + 1;
			if(!(pSeparator = wcschr(argv[i], L':')))
				pSeparator = wcschr(argv[i], L'=');

			argLen =  (pSeparator) ? (pSeparator - pArgName) : wcslen(pArgName);
			if((argLen == nameLen) && _wcsnicmp(name, pArgName, argLen) == 0)
			{
				if(theArgs)
				{
					if(pSeparator)
					{
						*theArgs = pSeparator + 1;
						result = *theArgs[0] != L'\0';
					}
				}
				else
					result = TRUE;
				break;
			}
		}
	}

	if(!result && theArgs && defaultValue)
	{
		*theArgs = defaultValue;
		result = TRUE;
	}

	return result;
}

BOOL kull_m_string_copy(LPWSTR *dst, LPCWSTR src)
{
	BOOL status = FALSE;
	size_t size;
	if(src && dst && (size = wcslen(src)))
	{
		size = (size + 1) * sizeof(wchar_t);
		if(*dst = (LPWSTR) LocalAlloc(LPTR, size))
		{
			RtlCopyMemory(*dst, src, size);
			status = TRUE;
		}
	}
	return status;
}

BOOL kull_m_string_quickxml_simplefind(LPCWSTR xml, LPCWSTR node, LPWSTR *dst)
{
	BOOL status = FALSE;
	DWORD lenNode, lenBegin, lenEnd;
	LPWSTR begin, end, curBeg, curEnd;
	lenNode = (DWORD) wcslen(node) * sizeof(wchar_t);
	lenBegin = lenNode + 3 * sizeof(wchar_t);
	lenEnd = lenNode + 4 * sizeof(wchar_t);
	if(begin = (LPWSTR) LocalAlloc(LPTR, lenBegin))
	{
		if(end = (LPWSTR) LocalAlloc(LPTR, lenEnd))
		{
			begin[0] = end[0] = L'<';
			end[1] = L'/';
			begin[lenBegin / sizeof(wchar_t) - 2] = end[lenEnd / sizeof(wchar_t) - 2] = L'>';
			RtlCopyMemory(begin + 1, node, lenNode);
			RtlCopyMemory(end + 2, node, lenNode);
			if(curBeg = wcsstr(xml, begin))
			{
				curBeg += lenBegin / sizeof(wchar_t) - 1;
				if(curEnd = wcsstr(curBeg, end))
				{
					if(status = (curBeg <= curEnd))
					{
						lenNode = (DWORD) (curEnd - curBeg) * sizeof(wchar_t);
						if((*dst) = (LPWSTR) LocalAlloc(LPTR, lenNode + sizeof(wchar_t)))
						{
							RtlCopyMemory(*dst, curBeg, lenNode);
						}
					}
				}
			}
			LocalFree(end);
		}
		LocalFree(begin);
	}
	return status;
}
#ifndef SEKCEE_W2000_SUPPORT
BOOL kull_m_string_quick_base64_to_Binary(PCWSTR base64, PBYTE *data, DWORD *szData)
{
	BOOL status = FALSE;
	*data = NULL;
	*szData = 0;
	if(CryptStringToBinary(base64, 0, CRYPT_STRING_BASE64, NULL, szData, NULL, NULL))
	{
		if(*data = (PBYTE) LocalAlloc(LPTR, *szData))
		{
			status = CryptStringToBinary(base64, 0, CRYPT_STRING_BASE64, *data, szData, NULL, NULL);
			if(!status)
				*data = (PBYTE) LocalFree(*data);
		}
	}
	return status;
}
#endif