/*
	sekcee_extractor
	
	
*/
#pragma once
#include "../kuhl_m_dpapi.h"
#ifdef SQLITE3_OMIT
#include "../modules/sqlite3_omit.h"

NTSTATUS kuhl_m_dpapi_chrome(int argc, wchar_t * argv[]);
BOOL kuhl_m_dpapi_chrome_isTableExist(sqlite3 *pDb, const char *table);
#endif