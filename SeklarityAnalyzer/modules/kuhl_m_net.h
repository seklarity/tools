/*
	sekcee_extractor
	
	
*/
#pragma once
#include "kuhl_m.h"
#include "../modules/kull_m_net.h"
#include "../modules/kull_m_token.h"
#include "../modules/kull_m_samlib.h"
#include "../modules/kull_m_string.h"

const KUHL_M kuhl_m_net;

NTSTATUS kuhl_m_net_user(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_net_group(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_net_alias(int argc, wchar_t * argv[]);

void kuhl_m_net_simpleLookup(SAMPR_HANDLE hDomainHandle, DWORD rid);

NTSTATUS kuhl_m_net_autoda(int argc, wchar_t * argv[]);