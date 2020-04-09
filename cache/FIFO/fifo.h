#ifndef FIFO_H
#define FIFO_H

typedef struct QUEUE_ENTRY{
	int data;
	struct QUEUE_ENTRY *next;
	struct QUEUE_ENTRY *prev;
}QUEUE_ENTRY;

typedef struct QUEUE{
	struct QUEUE_ENTRY* head;
	struct QUEUE_ENTRY* tail;
	int n_limit;
	int n_total;
}QUEUE;

void QUEUE_init(QUEUE *Q, int n_limit);
void QUEUE_show(QUEUE *Q);
void QUEUE_insert(QUEUE *Q, int data);
void QUEUE_finsert(QUEUE *Q, int data);
QUEUE_ENTRY* QUEUE_remove(QUEUE *Q, int index);

#endif
