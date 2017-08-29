/*
	sekcee_extractor
	
	
*/
#pragma once
#include "globals.h"
#include <io.h>
#include <fcntl.h>


FILE * logfile;
#ifdef _WINDLL
wchar_t * outputBuffer2;
HANDLE HnewHeap;
size_t outputBufferElements2, outputBufferElementsPosition2;


wchar_t * c_return_buffer;
size_t c_return_buffer_elements , c_return_buffer_elements_position, max_return_buffer;
#endif

void sekcee_stdout(PCWCHAR format, ...);

BOOL kull_m_output_file(PCWCHAR file);

void kull_m_output_init();
void kull_m_output_clean();