#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

QUEUE_ENTRY* QUEUE_ENTRY_init(int data, QUEUE_ENTRY *prev, QUEUE_ENTRY *next)
{
	QUEUE_ENTRY* QE = (QUEUE_ENTRY*)malloc(sizeof(QUEUE_ENTRY));
	QE->data = data;
	QE->next = next;
	QE->prev = prev;
	return QE;
}


void QUEUE_init(QUEUE *Q, int n_limit){ 
	// printf("init()\n");
	Q->n_limit = n_limit;
	Q->n_total = 0;
	Q->head = QUEUE_ENTRY_init(-1, NULL, NULL);
	Q->tail = QUEUE_ENTRY_init(-2, NULL, NULL);
}


void QUEUE_show(QUEUE *Q){ 
	// printf("show()\n");
	QUEUE_ENTRY* current = Q->head->next;
	while(current!=NULL)
	{	
		printf("[%2d]", current->data);
		current = current->next;
	}
	printf("\n");
}

void QUEUE_finsert(QUEUE *Q, int data)
{
	QUEUE_ENTRY* head = Q->head;
	QUEUE_ENTRY* tail = Q->tail;

	QUEUE_ENTRY* QE;
	if(Q->n_total == 0)
	{
		QE = QUEUE_ENTRY_init(data, NULL, NULL);
		head->next = QE;
		tail->prev = QE;
		Q->n_total += 1;
	}
	else if(Q->n_total < Q->n_limit)
	{
		QE = QUEUE_ENTRY_init(data, NULL, head->next);
		head->next->prev = QE;
		head->next = QE;
		Q->n_total += 1;
	}
	else if(Q->n_total == Q->n_limit)
	{
		QE = QUEUE_ENTRY_init(data, NULL, head->next);
		head->next->prev = QE;
		head->next = QE;
		// move tail backward by one-step
		QUEUE_ENTRY* QE1 = tail->prev;
		tail->prev = QE1->prev;
		head->prev->next = NULL;
		free(QE1);
	}
}

void QUEUE_insert(QUEUE *Q, int data)
{ 
	// printf("insert(%d)\n",data); 
	QUEUE_ENTRY* head = Q->head;
	QUEUE_ENTRY* tail = Q->tail;

	QUEUE_ENTRY* QE;
	if(Q->n_total == 0)
	{
		QE = QUEUE_ENTRY_init(data, NULL, NULL);
		head->next = QE;
		tail->prev = QE;
		Q->n_total += 1;
	}
	else if(Q->n_total < Q->n_limit)
	{
		QE = QUEUE_ENTRY_init(data, tail->prev, NULL);
		tail->prev->next = QE;
		tail->prev = QE;
		Q->n_total += 1;
	}
	else if(Q->n_total == Q->n_limit)
	{
		QE = QUEUE_ENTRY_init(data, tail->prev, NULL);
		tail->prev->next = QE;
		tail->prev = QE;
		// move head forward by one-step
		QUEUE_ENTRY* QE1 = head->next;
		head->next = QE1->next;
		head->next->prev = NULL;
		free(QE1);
	}
}


QUEUE_ENTRY* QUEUE_remove(QUEUE *Q, int index)
{ 
	// printf("remove(index=%d)\n", index);
	QUEUE_ENTRY* current = Q->head->next;
	for(int i=0; i<index; i++)
		current = current->next; 

	QUEUE_ENTRY *PREV, *NEXT;
	if(current!=Q->head->next && current!=Q->tail->prev)
	{
		PREV = current->prev;
		NEXT = current->next;
		PREV->next = current->next;
		NEXT->prev = current->prev;	
	}
	else if(current==Q->head->next && current!=Q->tail->prev)
	{
		PREV = Q->head;
		NEXT = current->next;
		PREV->next = current->next;
		NEXT->prev = NULL;
	}
	else if(current==Q->tail->prev && current!=Q->head->next)
	{
		PREV = current->prev;
		NEXT = Q->tail;
		PREV->next = NULL;
		NEXT->prev = current->prev;
	}
	else
	{
		// head == tail, queue becomes empty
		Q->head->next = NULL;
		Q->tail->prev = NULL;
	}
	current->next = NULL;
	current->prev = NULL;
	Q->n_total -= 1;
	return current;
}