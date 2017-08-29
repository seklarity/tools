/*
	sekcee_extractor
	
	
*/
#pragma once
#include "../kuhl_m_dpapi.h"
#include "../../kuhl_m_crypto.h"
#include "../modules/kull_m_key.h"

NTSTATUS kuhl_m_dpapi_keys_cng(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_dpapi_keys_capi(int argc, wchar_t * argv[]);