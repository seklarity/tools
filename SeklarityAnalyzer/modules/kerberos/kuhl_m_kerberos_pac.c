/*
	sekcee_extractor
	
	
*/
#include "kuhl_m_kerberos_pac.h"

BOOL kuhl_m_pac_validationInfo_to_PAC(PKERB_VALIDATION_INFO validationInfo, DWORD SignatureType, PCLAIMS_SET pClaimsSet, PPACTYPE * pacType, DWORD * pacLength)
{
	BOOL status = FALSE;
	PVOID pLogonInfo = NULL, pClaims = NULL;
	PPAC_CLIENT_INFO pClientInfo = NULL;
	PAC_SIGNATURE_DATA signature = {SignatureType, {0}};
	DWORD n = 4, szLogonInfo = 0, szLogonInfoAligned, szClientInfo = 0, szClientInfoAligned, szClaims = 0, szClaimsAligned = 0, szSignature = FIELD_OFFSET(PAC_SIGNATURE_DATA, Signature), szSignatureAligned, offsetData = sizeof(PACTYPE) + 3 * sizeof(PAC_INFO_BUFFER);
	PKERB_CHECKSUM pCheckSum;

	if(NT_SUCCESS(CDLocateCheckSum(SignatureType, &pCheckSum)))
	{
		szSignature += pCheckSum->Size;
		szSignatureAligned = SIZE_ALIGN(szSignature, 8);

		if(kull_m_pac_EncodeValidationInformation(&validationInfo, &pLogonInfo, &szLogonInfo))
			szLogonInfoAligned = SIZE_ALIGN(szLogonInfo, 8);
		if(kuhl_m_pac_validationInfo_to_CNAME_TINFO(validationInfo, &pClientInfo, &szClientInfo))
			szClientInfoAligned = SIZE_ALIGN(szClientInfo, 8);
		if(pClaimsSet)
			if(kuhl_m_kerberos_claims_encode_ClaimsSet(pClaimsSet, &pClaims, &szClaims))
			{
				szClaimsAligned = SIZE_ALIGN(szClaims, 8);
				n++;
				offsetData += sizeof(PAC_INFO_BUFFER);
			}

		if(pLogonInfo && pClientInfo)
		{
			*pacLength = offsetData + szLogonInfoAligned + szClientInfoAligned + szClaimsAligned + 2 * szSignatureAligned;
			if(*pacType = (PPACTYPE) LocalAlloc(LPTR, *pacLength))
			{
				(*pacType)->cBuffers = n;
				(*pacType)->Version = 0;

				(*pacType)->Buffers[0].cbBufferSize = szLogonInfo;
				(*pacType)->Buffers[0].ulType = PACINFO_TYPE_LOGON_INFO;
				(*pacType)->Buffers[0].Offset = offsetData;
				RtlCopyMemory((PBYTE) *pacType + (*pacType)->Buffers[0].Offset, pLogonInfo, (*pacType)->Buffers[0].cbBufferSize);

				(*pacType)->Buffers[1].cbBufferSize = szClientInfo;
				(*pacType)->Buffers[1].ulType = PACINFO_TYPE_CNAME_TINFO;
				(*pacType)->Buffers[1].Offset = (*pacType)->Buffers[0].Offset + szLogonInfoAligned;
				RtlCopyMemory((PBYTE) *pacType + (*pacType)->Buffers[1].Offset, pClientInfo, (*pacType)->Buffers[1].cbBufferSize);

				if(szClaimsAligned)
				{
					(*pacType)->Buffers[2].cbBufferSize = szClaims;
					(*pacType)->Buffers[2].ulType = PACINFO_TYPE_CLIENT_CLAIMS;
					(*pacType)->Buffers[2].Offset = (*pacType)->Buffers[1].Offset + szClientInfoAligned;
					RtlCopyMemory((PBYTE) *pacType + (*pacType)->Buffers[2].Offset, pClaims, (*pacType)->Buffers[2].cbBufferSize);
				}

				(*pacType)->Buffers[n - 2].cbBufferSize = szSignature;
				(*pacType)->Buffers[n - 2].ulType = PACINFO_TYPE_CHECKSUM_SRV;
				(*pacType)->Buffers[n - 2].Offset = (*pacType)->Buffers[n - 3].Offset + SIZE_ALIGN((*pacType)->Buffers[n - 3].cbBufferSize, 8);
				RtlCopyMemory((PBYTE) *pacType + (*pacType)->Buffers[n - 2].Offset, &signature, FIELD_OFFSET(PAC_SIGNATURE_DATA, Signature));

				(*pacType)->Buffers[n - 1].cbBufferSize = szSignature;
				(*pacType)->Buffers[n - 1].ulType = PACINFO_TYPE_CHECKSUM_KDC;
				(*pacType)->Buffers[n - 1].Offset = (*pacType)->Buffers[n - 2].Offset + szSignatureAligned;
				RtlCopyMemory((PBYTE) *pacType + (*pacType)->Buffers[n - 1].Offset, &signature, FIELD_OFFSET(PAC_SIGNATURE_DATA, Signature));

				status = TRUE;
			}
		}

		if(pLogonInfo)
			LocalFree(pLogonInfo);
		if(pClientInfo)
			LocalFree(pClientInfo);
		if(pClaims)
			LocalFree(pClaims);
	}
	return status;
}

NTSTATUS kuhl_m_pac_signature(PPACTYPE pacType, DWORD pacLenght, DWORD SignatureType, LPCVOID key, DWORD keySize)
{
	NTSTATUS status;
	DWORD i;
	PKERB_CHECKSUM pCheckSum;
	PVOID Context;
	PPAC_SIGNATURE_DATA pSignatureData;
	PBYTE checksumSrv = NULL, checksumpKdc = NULL;

	status = CDLocateCheckSum(SignatureType, &pCheckSum);
	if(NT_SUCCESS(status))
	{
		status = STATUS_NOT_FOUND;
		for(i = 0; i < pacType->cBuffers; i++)
		{
			if((pacType->Buffers[i].ulType == PACINFO_TYPE_CHECKSUM_SRV) || (pacType->Buffers[i].ulType == PACINFO_TYPE_CHECKSUM_KDC))
			{
				pSignatureData = (PPAC_SIGNATURE_DATA) ((PBYTE) pacType + pacType->Buffers[i].Offset);
				RtlZeroMemory(pSignatureData->Signature, pCheckSum->Size);
				if(pacType->Buffers[i].ulType ==  PACINFO_TYPE_CHECKSUM_SRV)
					checksumSrv = pSignatureData->Signature;
				else
					checksumpKdc = pSignatureData->Signature;
			}
		}
		if(checksumSrv && checksumpKdc)
		{
			status = pCheckSum->InitializeEx(key, keySize, KERB_NON_KERB_CKSUM_SALT, &Context);
			if(NT_SUCCESS(status))
			{
				pCheckSum->Sum(Context, pacLenght, pacType);
				pCheckSum->Finalize(Context, checksumSrv);
				pCheckSum->Finish(&Context);
				status = pCheckSum->InitializeEx(key, keySize, KERB_NON_KERB_CKSUM_SALT, &Context);
				if(NT_SUCCESS(status))
				{
					pCheckSum->Sum(Context, pCheckSum->Size, checksumSrv);
					pCheckSum->Finalize(Context, checksumpKdc);
					pCheckSum->Finish(&Context);
				}
			}
		}
	}
	return status;
}

BOOL kuhl_m_pac_validationInfo_to_CNAME_TINFO(PKERB_VALIDATION_INFO validationInfo, PPAC_CLIENT_INFO * pacClientInfo, DWORD * pacClientInfoLength)
{
	BOOL status = FALSE;
	*pacClientInfoLength = sizeof(PAC_CLIENT_INFO) + validationInfo->EffectiveName.Length - sizeof(wchar_t);
	if(*pacClientInfo = (PPAC_CLIENT_INFO) LocalAlloc(LPTR, *pacClientInfoLength))
	{
		(*pacClientInfo)->ClientId = validationInfo->LogonTime;
		(*pacClientInfo)->NameLength = validationInfo->EffectiveName.Length;
		RtlCopyMemory((*pacClientInfo)->Name, validationInfo->EffectiveName.Buffer, (*pacClientInfo)->NameLength);
		status = TRUE;
	}
	return status;
}

#ifdef KERBEROS_TOOLS
NTSTATUS kuhl_m_kerberos_pac_info(int argc, wchar_t * argv[])
{
	PPACTYPE pacType;
	DWORD pacLenght, i, j;
	PKERB_VALIDATION_INFO pValInfo = NULL;
	PPAC_SIGNATURE_DATA pSignatureData;
	PPAC_CLIENT_INFO pClientInfo;
	PUPN_DNS_INFO pUpnDnsInfo;
	PCLAIMS_SET_METADATA pClaimsSetMetadata = NULL;
	PCLAIMS_SET claimsSet = NULL;

	if(argc)
	{
		if(kull_m_file_readData(argv[0], (PBYTE *) &pacType, &pacLenght))
		{
			sekcee_stdout(L"version %u, nbBuffer = %u\n\n", pacType->Version, pacType->cBuffers);

			for(i = 0; i < pacType->cBuffers; i++)
			{
				switch(pacType->Buffers[i].ulType)
				{
				case PACINFO_TYPE_LOGON_INFO:
					sekcee_stdout(L"*** Validation Informations *** (%u)\n", pacType->Buffers[i].cbBufferSize);
					if(kull_m_pac_DecodeValidationInformation((PBYTE) pacType + pacType->Buffers[i].Offset, pacType->Buffers[i].cbBufferSize, &pValInfo))
					{
						sekcee_stdout(L"LogonTime              %016llx - ", pValInfo->LogonTime); kull_m_string_displayLocalFileTime(&pValInfo->LogonTime); sekcee_stdout(L"\n");
						sekcee_stdout(L"LogoffTime             %016llx - ", pValInfo->LogoffTime); kull_m_string_displayLocalFileTime(&pValInfo->LogoffTime); sekcee_stdout(L"\n");
						sekcee_stdout(L"KickOffTime            %016llx - ", pValInfo->KickOffTime); kull_m_string_displayLocalFileTime(&pValInfo->KickOffTime); sekcee_stdout(L"\n");
						sekcee_stdout(L"PasswordLastSet        %016llx - ", pValInfo->PasswordLastSet); kull_m_string_displayLocalFileTime(&pValInfo->PasswordLastSet); sekcee_stdout(L"\n");
						sekcee_stdout(L"PasswordCanChange      %016llx - ", pValInfo->PasswordCanChange); kull_m_string_displayLocalFileTime(&pValInfo->PasswordCanChange); sekcee_stdout(L"\n");
						sekcee_stdout(L"PasswordMustChange     %016llx - ", pValInfo->PasswordMustChange); kull_m_string_displayLocalFileTime(&pValInfo->PasswordMustChange); sekcee_stdout(L"\n");
						sekcee_stdout(L"EffectiveName          %wZ\n", &pValInfo->EffectiveName);
						sekcee_stdout(L"FullName               %wZ\n", &pValInfo->FullName);
						sekcee_stdout(L"LogonScript            %wZ\n", &pValInfo->LogonScript);
						sekcee_stdout(L"ProfilePath            %wZ\n", &pValInfo->ProfilePath);
						sekcee_stdout(L"HomeDirectory          %wZ\n", &pValInfo->HomeDirectory);
						sekcee_stdout(L"HomeDirectoryDrive     %wZ\n", &pValInfo->HomeDirectoryDrive);
						sekcee_stdout(L"LogonCount             %hu\n", pValInfo->LogonCount);
						sekcee_stdout(L"BadPasswordCount       %hu\n", pValInfo->BadPasswordCount);
						sekcee_stdout(L"UserId                 %08x (%u)\n", pValInfo->UserId, pValInfo->UserId);
						sekcee_stdout(L"PrimaryGroupId         %08x (%u)\n", pValInfo->PrimaryGroupId, pValInfo->PrimaryGroupId);
						sekcee_stdout(L"GroupCount             %u\n", pValInfo->GroupCount);
						sekcee_stdout(L"GroupIds               ");
						for(j = 0; j < pValInfo->GroupCount; j++)
							sekcee_stdout(L"%u, ", pValInfo->GroupIds[j].RelativeId); //, pGroup[j].Attributes);
						sekcee_stdout(L"\n");
						sekcee_stdout(L"UserFlags              %08x (%u)\n", pValInfo->UserFlags, pValInfo->UserFlags);
						sekcee_stdout(L"UserSessionKey         "); kull_m_string_wprintf_hex(pValInfo->UserSessionKey.data, 16, 0); sekcee_stdout(L"\n");
						sekcee_stdout(L"LogonServer            %wZ\n", &pValInfo->LogonServer);
						sekcee_stdout(L"LogonDomainName        %wZ\n", &pValInfo->LogonDomainName);
						sekcee_stdout(L"LogonDomainId          "); kull_m_string_displaySID(pValInfo->LogonDomainId); sekcee_stdout(L"\n");
						sekcee_stdout(L"UserAccountControl     %08x (%u)\n", pValInfo->UserAccountControl, pValInfo->UserAccountControl);
						sekcee_stdout(L"SubAuthStatus          %08x (%u)\n", pValInfo->SubAuthStatus, pValInfo->SubAuthStatus);
						sekcee_stdout(L"LastSuccessfulILogon   %016llx - ", pValInfo->LastSuccessfulILogon); kull_m_string_displayLocalFileTime(&pValInfo->LastSuccessfulILogon); sekcee_stdout(L"\n");
						sekcee_stdout(L"LastFailedILogon       %016llx - ", pValInfo->LastFailedILogon); kull_m_string_displayLocalFileTime(&pValInfo->LastFailedILogon); sekcee_stdout(L"\n");
						sekcee_stdout(L"FailedILogonCount      %08x (%u)\n", pValInfo->FailedILogonCount, pValInfo->FailedILogonCount);
						sekcee_stdout(L"SidCount               %u\n", pValInfo->SidCount);
						sekcee_stdout(L"ExtraSids\n");
						for(j = 0; j < pValInfo->SidCount; j++)
						{
							sekcee_stdout(L"  ");
							kull_m_string_displaySID(pValInfo->ExtraSids[j].Sid);
							sekcee_stdout(L"\n");
						}
						sekcee_stdout(L"ResourceGroupDomainSid "); if(pValInfo->ResourceGroupDomainSid) kull_m_string_displaySID(pValInfo->ResourceGroupDomainSid); sekcee_stdout(L"\n");
						sekcee_stdout(L"ResourceGroupCount     %u\n", pValInfo->ResourceGroupCount);
						sekcee_stdout(L"ResourceGroupIds       ");
						for(j = 0; j < pValInfo->ResourceGroupCount; j++)
							sekcee_stdout(L"%u, ", pValInfo->ResourceGroupIds[j].RelativeId); //, pGroup[j].Attributes);
						sekcee_stdout(L"\n");
						kull_m_pac_FreeValidationInformation(&pValInfo);
					}
					break;
				case PACINFO_TYPE_CHECKSUM_SRV: // Server Signature
				case PACINFO_TYPE_CHECKSUM_KDC: // KDC Signature
					pSignatureData = (PPAC_SIGNATURE_DATA) ((PBYTE) pacType + pacType->Buffers[i].Offset);
					sekcee_stdout(L"*** %s Signature ***\n", (pacType->Buffers[i].ulType == PACINFO_TYPE_CHECKSUM_SRV) ? L"Server" : L"KDC");
					sekcee_stdout(L"Type %08x - (%hu) : ", pSignatureData->SignatureType, 0);//pSignatureData->RODCIdentifier);
					kull_m_string_wprintf_hex(pSignatureData->Signature, (pSignatureData->SignatureType == KERB_CHECKSUM_HMAC_MD5) ? LM_NTLM_HASH_LENGTH : 12, 0);
					sekcee_stdout(L"\n");
					break;
				case PACINFO_TYPE_CNAME_TINFO: // Client name and ticket information
					pClientInfo  = (PPAC_CLIENT_INFO) ((PBYTE) pacType + pacType->Buffers[i].Offset);
					sekcee_stdout(L"*** Client name and ticket information ***\n");
					sekcee_stdout(L"ClientId %016llx - ", pClientInfo->ClientId); kull_m_string_displayLocalFileTime(&pClientInfo->ClientId); sekcee_stdout(L"\n");
					sekcee_stdout(L"Client   %.*s\n", pClientInfo->NameLength / sizeof(WCHAR), pClientInfo->Name);
					break;
				case PACINFO_TYPE_UPN_DNS:
					pUpnDnsInfo = (PUPN_DNS_INFO) ((PBYTE) pacType + pacType->Buffers[i].Offset);
					sekcee_stdout(L"*** UPN and DNS information ***\n");
					kull_m_string_wprintf_hex(pUpnDnsInfo, pacType->Buffers[i].cbBufferSize, 2);
					sekcee_stdout(L"UPN            %.*s\n", pUpnDnsInfo->UpnLength / sizeof(WCHAR), (PBYTE) pUpnDnsInfo + pUpnDnsInfo->UpnOffset);
					sekcee_stdout(L"DnsDomainName  %.*s\n", pUpnDnsInfo->DnsDomainNameLength / sizeof(WCHAR), (PBYTE) pUpnDnsInfo + pUpnDnsInfo->DnsDomainNameOffset);
					sekcee_stdout(L"Flags          %08x (%u)\n", pUpnDnsInfo->Flags, pUpnDnsInfo->Flags);
					break;
				case PACINFO_TYPE_CLIENT_CLAIMS:
				case PACINFO_TYPE_DEVICE_CLAIMS:
					sekcee_stdout(L"*** %s claims Informations *** (%u)\n", (pacType->Buffers[i].ulType == PACINFO_TYPE_CLIENT_CLAIMS) ? L"Client" : L"Device", pacType->Buffers[i].cbBufferSize);
					if(pacType->Buffers[i].cbBufferSize)
					{
						kull_m_string_wprintf_hex((PBYTE) pacType + pacType->Buffers[i].Offset, pacType->Buffers[i].cbBufferSize, 2);
						if(kull_m_rpc_DecodeClaimsSetMetaData((PBYTE) pacType + pacType->Buffers[i].Offset, pacType->Buffers[i].cbBufferSize, &pClaimsSetMetadata))
						{
							if(pClaimsSetMetadata->usCompressionFormat == CLAIMS_COMPRESSION_FORMAT_NONE)
							{
								if(kull_m_rpc_DecodeClaimsSet(pClaimsSetMetadata->ClaimsSet, pClaimsSetMetadata->ulUncompressedClaimsSetSize, &claimsSet))
								{
									kuhl_m_kerberos_claims_displayClaimsSet(claimsSet);
									kull_m_rpc_FreeClaimsSet(&claimsSet);
								}
							}
							else PRINT_ERROR(L"Compression not supported (%hu)\n", pClaimsSetMetadata->usCompressionFormat);
							kull_m_rpc_FreeClaimsSetMetaData(&pClaimsSetMetadata);
						}
					}
					break;
				default:
					kull_m_string_wprintf_hex(&pacType->Buffers[i], sizeof(PAC_INFO_BUFFER), 1);
					sekcee_stdout(L"\n");
					sekcee_stdout(L"[%02u] %08x @ offset %016llx (%u)\n", i, pacType->Buffers[i].ulType, pacType->Buffers[i].Offset, pacType->Buffers[i].cbBufferSize);
					kull_m_string_wprintf_hex((PBYTE) pacType + pacType->Buffers[i].Offset, pacType->Buffers[i].cbBufferSize, 1);
					sekcee_stdout(L"\n");
				}
				sekcee_stdout(L"\n");
			}
			LocalFree(pacType);
		}
		else PRINT_ERROR_AUTO(L"kull_m_file_readData");
	}
	return STATUS_SUCCESS;
}
#endif