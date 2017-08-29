/*
	sekcee_extractor
	
	
*/
#pragma once
#include "../kuhl_m.h"
#include "kuhl_m_kerberos_claims.h"
#include "../modules/kull_m_file.h"
#include "../modules/kull_m_crypto_system.h"
#include "../modules/rpc/kull_m_rpc_ms-pac.h"

#define KERB_NON_KERB_SALT					16
#define KERB_NON_KERB_CKSUM_SALT			17

#define PACINFO_TYPE_LOGON_INFO				0x00000001
#define PACINFO_TYPE_CHECKSUM_SRV			0x00000006
#define PACINFO_TYPE_CHECKSUM_KDC			0x00000007
#define PACINFO_TYPE_CNAME_TINFO			0x0000000a
#define PACINFO_TYPE_UPN_DNS				0x0000000c
#define PACINFO_TYPE_CLIENT_CLAIMS			0x0000000d
#define PACINFO_TYPE_DEVICE_CLAIMS			0x0000000f

typedef struct _PAC_INFO_BUFFER {
	ULONG ulType;
	ULONG cbBufferSize;
	ULONG64 Offset;
} PAC_INFO_BUFFER, *PPAC_INFO_BUFFER;

typedef struct _PACTYPE {
	ULONG cBuffers;
	ULONG Version;
	PAC_INFO_BUFFER Buffers[ANYSIZE_ARRAY];
} PACTYPE, *PPACTYPE;

#pragma pack(push, 4) 
typedef struct _PAC_SIGNATURE_DATA {
	ULONG  SignatureType;
	UCHAR  Signature[ANYSIZE_ARRAY];// LM_NTLM_HASH_LENGTH];
	//USHORT RODCIdentifier;
	//USHORT  Reserverd;
} PAC_SIGNATURE_DATA, *PPAC_SIGNATURE_DATA;
#pragma pack(pop)

typedef struct _PAC_CLIENT_INFO {
	FILETIME ClientId;
	USHORT NameLength;
	WCHAR Name[ANYSIZE_ARRAY];
} PAC_CLIENT_INFO, *PPAC_CLIENT_INFO;

typedef struct _UPN_DNS_INFO {
	USHORT UpnLength;
	USHORT UpnOffset;
	USHORT DnsDomainNameLength;
	USHORT DnsDomainNameOffset;
	ULONG Flags;
} UPN_DNS_INFO, *PUPN_DNS_INFO;

BOOL kuhl_m_pac_validationInfo_to_PAC(PKERB_VALIDATION_INFO validationInfo, DWORD SignatureType, PCLAIMS_SET pClaimsSet, PPACTYPE *pacType, DWORD *pacLength);
BOOL kuhl_m_pac_validationInfo_to_CNAME_TINFO(PKERB_VALIDATION_INFO validationInfo, PPAC_CLIENT_INFO *pacClientInfo, DWORD *pacClientInfoLength);
NTSTATUS kuhl_m_pac_signature(PPACTYPE pacType, DWORD pacLenght, DWORD SignatureType, LPCVOID key, DWORD keySize);

#ifdef KERBEROS_TOOLS
NTSTATUS kuhl_m_kerberos_pac_info(int argc, wchar_t * argv[]);
#endif