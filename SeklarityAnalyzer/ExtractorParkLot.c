#include "SKA.h"

void doSomething()
{
	printf("doing something - msg from extractor\n ");
#ifdef _WINDLL
	printf("ifdef windll is TRUE");

#endif;
}

__declspec(dllexport) SampleStruct* c_dllcall_for_structs(int* noofrecs)
{
	printf("Inside called_c_string_function\n");

	static SampleStruct a[] =
	{
		{ "id1", "desc1" },
		{ "id2", "desc2" },
		{ "id3", "desc3" },
		{ "id4", "desc4" },
		{ "id5", "desc5" },
		{ "id1", "desc1" },
		{ "id2", "desc2" },
		{ "id3", "desc3" },
		{ "id4", "desc4" },
		{ "id5", "desc5" }
	};
	*noofrecs = sizeof(a) / sizeof(a[0]);
	return a;

}

__declspec(dllexport) wchar_t* dllcall()
{
	doSomething();
	printf("pre bufferele created");
	outputBufferElements2 = 0xff;
	outputBufferElementsPosition2 = 0;
	printf("post bufferle created");
	//wchar_t *message;
	//message = (wchar_t *)malloc(sizeof(wchar_t) * 100);
	printf("pre heap created \n");
	HnewHeap = HeapCreate(0, 255, 60000);
	printf("heap created \n");
	if (outputBuffer2 = (wchar_t *)HeapAlloc(HnewHeap, 0, 60000))
		//if (outputBuffer2 = (wchar_t *)malloc(outputBufferElements2))
	{
		printf("heapappcated too \n");
		int status = STATUS_SUCCESS;

		sekcee_initOrClean(TRUE);
		status = kuhl_m_privilege_debug_all();
		status = kuhl_m_sekurlsa_all();
		sekcee_initOrClean(FALSE);
	}

	return outputBuffer2;
}

__declspec(dllexport) wchar_t* csharp_dllcall_for_stringbuffer()
{
	printf("Inside called_c_string_function");

	//Init the heap to write 10 elements
	c_return_buffer_elements_position = 0;
	c_return_buffer_elements = 10;
	max_return_buffer = 40;

	HnewHeap = HeapCreate(0, c_return_buffer_elements, max_return_buffer);

	if (c_return_buffer = (wchar_t *)HeapAlloc(HnewHeap, 0, c_return_buffer_elements))
	{
		printf("heap allocated \n");
		int status = STATUS_SUCCESS;
		for (int i = 0; i <= 200; i++)
		{
			sekcee_stdout(L"%d-0123456\n", i);
		}

		//sekcee_initOrClean(TRUE);
		//status = kuhl_m_privilege_debug_all();
		//status = kuhl_m_sekurlsa_all();
		//sekcee_initOrClean(FALSE);
	}

	return c_return_buffer;
}