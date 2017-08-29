/*	Benjamin DELPY `gentilkiwi`
http://blog.gentilkiwi.com
benjamin@gentilkiwi.com
Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kull_m_output.h"

wchar_t* ws1 = L"";

FILE * logfile = NULL;
#ifdef _WINDLL
wchar_t * outputBuffer2 = NULL;
size_t outputBufferElements2 = 0, outputBufferElementsPosition2 = 0;
HANDLE HnewHeap;

wchar_t * c_return_buffer = NULL;
size_t c_return_buffer_elements = 0, c_return_buffer_elements_position = 0, max_return_buffer = 0;

#endif

void sekcee_stdout(PCWCHAR format, ...)
{
/*
#ifndef _WINDLL
	int varBuf;
	size_t tempSize;
#endif
	va_list args;
	va_start(args, format);
#ifdef _WINDLL
	vwprintf(format, args);
	fflush(stdout);

#else
	if (outputBuffer2)
	{
		varBuf = _vscwprintf(format, args);
		if (varBuf > 0)
		{
			if ((size_t)varBuf > (outputBufferElements2 - outputBufferElementsPosition2 - 1)) // NULL character
			{
				tempSize = (outputBufferElements2 + varBuf + 1) * 2;// just to be cool
				printf("writing to bufer elemet at %zu \n", outputBufferElements2);
				if (outputBuffer2 = (wchar_t *)HeapReAlloc(HnewHeap,0, outputBuffer2, tempSize * sizeof(wchar_t)))
					outputBufferElements2 = tempSize;
			}
			varBuf = vswprintf(outputBuffer2 + outputBufferElementsPosition2, outputBufferElements2 - outputBufferElementsPosition2, format, args);
			if (varBuf > 0)
				outputBufferElementsPosition2 += varBuf;
		}
	}
#endif
	if (logfile)
		vfwprintf(logfile, format, args);
	va_end(args);
	fflush(logfile);
*/
}

void sekcee_stdout_new(PCWCHAR format, ...)
{
	int varBuf;
	size_t tempSize;
	va_list args;
	va_start(args, format);

	if (c_return_buffer)
	{
		varBuf = _vscwprintf(format, args);
		if (varBuf > 0)
		{
			if ((size_t)varBuf > (c_return_buffer_elements - c_return_buffer_elements_position - 1)) // subtract for null char
			{
				tempSize = (c_return_buffer_elements + varBuf + 1) * 2;// just to be cool

				if (c_return_buffer = (wchar_t *)HeapReAlloc(HnewHeap, 0, c_return_buffer, tempSize * sizeof(wchar_t)))
					c_return_buffer_elements = tempSize;
			}
			printf("NO of buffer elements %zu \n", c_return_buffer_elements_position);
			varBuf = vswprintf(c_return_buffer + c_return_buffer_elements_position, c_return_buffer_elements - c_return_buffer_elements_position, format, args);
			if (varBuf > 0)
				c_return_buffer_elements_position += varBuf;
		}
	}
}

void kprintf_inputline(PCWCHAR format, ...)
{
	va_list args;
	va_start(args, format);
	if (logfile)
		vfwprintf(logfile, format, args);
	va_end(args);
	fflush(logfile);
}

BOOL kull_m_output_file(PCWCHAR file)
{
	BOOL status = FALSE;
	FILE * newlog = NULL;

	if (file)
#pragma warning(push)
#pragma warning(disable:4996)
		newlog = _wfopen(file, L"a"); // XP does not like _wfopen_s
#pragma warning(pop)
	if (newlog || !file)
	{
		if (logfile)
			fclose(logfile);
		logfile = newlog;
	}
	return (!file || (file && logfile));
}

int previousStdOut, previousStdErr;
UINT previousConsoleOutput;
void kull_m_output_init()
{
	previousStdOut = _setmode(_fileno(stdout), _O_U8TEXT);
	previousStdErr = _setmode(_fileno(stderr), _O_U8TEXT);
	previousConsoleOutput = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);
}

void kull_m_output_clean()
{
	_setmode(_fileno(stdout), previousStdOut);
	_setmode(_fileno(stderr), previousStdErr);
	SetConsoleOutputCP(previousConsoleOutput);
}
