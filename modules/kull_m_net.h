/*
	sekcee_extractor
	
	
*/
#pragma once
#include "globals.h"
#include <DsGetDC.h>

extern DWORD WINAPI NetApiBufferFree (IN LPVOID Buffer);

BOOL kull_m_net_getCurrentDomainInfo(PPOLICY_DNS_DOMAIN_INFO * pDomainInfo);
BOOL kull_m_net_CreateWellKnownSid(WELL_KNOWN_SID_TYPE WellKnownSidType, PSID DomainSid, PSID * pSid);
BOOL kull_m_net_getDC(LPCWSTR fullDomainName, DWORD altFlags, LPWSTR * fullDCName);