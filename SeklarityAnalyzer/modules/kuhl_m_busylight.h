/*
	sekcee_extractor
	
	
*/
#pragma once
#include "kuhl_m.h"
#include "../modules/kull_m_busylight.h"
#include "../modules/kull_m_string.h"

const KUHL_M kuhl_m_busylight;

NTSTATUS kuhl_m_busylight_init();
NTSTATUS kuhl_m_busylight_clean();

NTSTATUS kuhl_m_busylight_status(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_busylight_list(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_busylight_off(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_busylight_single(int argc, wchar_t * argv[]);

NTSTATUS kuhl_m_busylight_test(int argc, wchar_t * argv[]);