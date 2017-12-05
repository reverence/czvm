/*
 * list.c
 *
 * Author: chengzhang
 */

#include "list.h"
#include "czvm.h"

int init_jvm_class_list(CLASS** klass,list* p)
{
	list* h = p;
	while(NULL != h->next)
	{
		h = h->next;
	}

	list* l = (list*)malloc(sizeof(list));
	if(!l)
		return -1;
	l->next = NULL;
	l->data = (*klass);
	h->next = l;
	l->prev = h;

	return 0;
}
