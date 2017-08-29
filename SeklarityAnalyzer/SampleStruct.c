//Seklarity.com 
#include "SKA.h"
extern const KUHL_M * sekcee_modules;
//-start  linkedlist func
struct SampleStruct *head = NULL;
struct SampleStruct *curr = NULL;
struct SampleStruct *ptr = NULL;

struct SampleStruct* create_list(int val)
{
	printf("\n creating list with headnode as [%d]\n", val);
	struct SampleStruct *ptr = (struct SampleStruct*)malloc(sizeof(struct SampleStruct));
	if (NULL == ptr)
	{
		printf("\n Node creation failed \n");
		return NULL;
	}
	ptr->val = val;
	ptr->next = NULL;

	head = curr = ptr;
	return ptr;
}

struct SampleStruct* add_to_list(int val, BOOL add_to_end)
{
	if (NULL == head)
	{
		return (create_list(val));
	}

	if (add_to_end)
		printf("\n Adding node to end of list with value [%d]\n", val);
	else
		printf("\n Adding node to beginning of list with value [%d]\n", val);

	struct SampleStruct *ptr = (struct SampleStruct*)malloc(sizeof(struct SampleStruct));
	if (NULL == ptr)
	{
		printf("\n Node creation failed \n");
		return NULL;
	}
	ptr->val = val;
	ptr->id = "id";
	ptr->username = "usr";
	ptr->next = NULL;

	if (add_to_end)
	{
		curr->next = ptr;
		curr = ptr;
	}
	else
	{
		ptr->next = head;
		head = ptr;
	}
	return ptr;
}

struct SampleStruct* search_in_list(int val, struct SampleStruct **prev)
{
	struct SampleStruct *ptr = head;
	struct SampleStruct *tmp = NULL;
	BOOL found = FALSE;

	printf("\n Searching the list for value [%d] \n", val);

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

int delete_from_list(int val)
{
	struct SampleStruct *prev = NULL;
	struct SampleStruct *del = NULL;

	printf("\n Deleting value [%d] from list\n", val);

	del = search_in_list(val, &prev);
	if (del == NULL)
	{
		return -1;
	}
	else
	{
		if (prev != NULL)
			prev->next = del->next;

		if (del == curr)
		{
			curr = prev;
		}
		else if (del == head)
		{
			head = del->next;
		}
	}

	free(del);
	del = NULL;

	return 0;
}

void print_list(void)
{
	struct SampleStruct *ptr = head;

	printf("\n -------Printing list Start------- \n");
	while (ptr != NULL)
	{
		printf("\n [%d] \n", ptr->val);
		ptr = ptr->next;
	}
	printf("\n -------Printing list End------- \n");

	return;
}

int count_list(void)
{
	int count = 0;
	struct SampleStruct *ptr = head;

	while (ptr != NULL)
	{
		count++;
		ptr = ptr->next;
	}
	return count;
}
//-end linkedlist func

__declspec(dllexport) SampleStruct*  GetLogons()
{
	printf("Inside called_c_dllcall_for_logons_function.\n");

	//llist
	int i = 0, ret = 0;

	print_list();

	for (i = 5; i<10; i++)
		add_to_list(i, TRUE);

	print_list();

	for (i = 4; i>0; i--)
		add_to_list(i, FALSE);

	print_list();

	for (i = 1; i<10; i += 4)
	{
		ptr = search_in_list(i, NULL);
		if (NULL == ptr)
		{
			printf("\n Search [val = %d] failed, no such element found\n", i);
		}
		else
		{
			printf("\n Search passed [val = %d]\n", ptr->val);
		}

		print_list();

		ret = delete_from_list(i);
		if (ret != 0)
		{
			printf("\n delete [val = %d] failed, no such element found\n", i);
		}
		else
		{
			printf("\n delete [val = %d]  passed \n", i);
		}

		print_list();


	}

	return head;
}