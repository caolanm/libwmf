/*very simple to use look at main.c to find out how*/


#include <stdlib.h>
#include "stack.h"

void init_c_stk(c_stack *stack)
	{
	stack->length=0;
	stack->fpointer=NULL;
	}

int len_c_stk(c_stack *stack)
	{
	return(stack->length);
	}

int push_c_stk(void *info,c_stack *stack)
	{
	if (stack->fpointer == NULL)
		{
		stack->fpointer = (link_c*)malloc(sizeof(link_c));
		if (stack->fpointer == NULL)
			return(1);
		stack->fpointer->last=NULL;
		stack->fpointer->next=NULL;
		}
	else
		{
		stack->fpointer->next = (link_c*)malloc(sizeof(link_c));
		if (stack->fpointer->next == NULL)
			return(1);
		stack->fpointer->next->last=stack->fpointer;
		stack->fpointer = stack->fpointer->next;
		}
	stack->fpointer->info=info;
	stack->length++;

	stack->bpointer = stack->fpointer;
	return(0);
	}

void *pop_c_stk(c_stack *stack)
	{
	void *ret;
	if (stack->bpointer==NULL)
		return(NULL);

	ret=stack->bpointer->info;

	if (stack->bpointer->last!=NULL)
		{
		stack->bpointer=stack->bpointer->last;
		free(stack->bpointer->next);
		stack->bpointer->next=NULL;
		}
	else
		{
		free(stack->bpointer);
		stack->bpointer=NULL;
		}
	
	stack->fpointer = stack->bpointer;
	stack->length--;
	return(ret);
	}

