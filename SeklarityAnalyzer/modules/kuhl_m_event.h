/*
	sekcee_extractor
	
	
*/
#pragma once
#include "kuhl_m.h"
#include "../modules/kull_m_process.h"
#include "../modules/kull_m_service.h"
#include "../modules/kull_m_memory.h"
#include "../modules/kull_m_patch.h"

const KUHL_M kuhl_m_event;

NTSTATUS kuhl_m_event_drop(int argc, wchar_t * argv[]);
NTSTATUS kuhl_m_event_clear(int argc, wchar_t * argv[]);