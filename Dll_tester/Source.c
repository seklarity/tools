#include <stdio.h>
extern wchar_t* dllcall();
extern int* c_dllcall_for_structs(int* noofrecs);

typedef struct SampleStruct
{
	char* id;
	char* username;
} SampleStruct;

typedef struct LogonStruct
{
	wchar_t* id;
	wchar_t*  username;
	int val;
	int tp;
	struct LogonStruct *next;
} LogonStruct;


extern LogonStruct* c_dllcall_for_logons(int* noofrecs);

int main()
{
	printf("hello");
	//wchar_t* pret = dllcall();
	//wprintf_s(pret);

	int s = 0;
	LogonStruct* ret2 = c_dllcall_for_logons(&s);
	printf("%d", 1);
	return 0;

}