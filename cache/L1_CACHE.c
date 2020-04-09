#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h> 
#include "L1_CACHE.h"

#define HIT set_i_row_index
#define MISS NULL


void L1_CACHE_init  (L1_CACHE *cache/*, int n_sets, int n_rows*/)
{
	cache->n_sets = 4; // n_sets;
	cache->n_rows = 128; // n_rows;
	cache->set = (L1_CACHE_SET*)malloc(sizeof(L1_CACHE_SET) * cache->n_sets);
	cache->hits = 0;
	cache->misses = 0;
	// initialize fifo datastructure for each row
	cache->fifo = (QUEUE**)malloc(sizeof(QUEUE*)*cache->n_rows);
	for(int j=0; j < cache->n_rows; ++j)
	{
		cache->fifo[j] = (QUEUE*)malloc(sizeof(QUEUE));
		QUEUE_init(cache->fifo[j], cache->n_sets);
		
		for(int k=0; k<cache->n_sets; k++)
			QUEUE_insert(cache->fifo[j], k);
		// cache->fifo[j] = [0][1][2][3]
	}

	// initialize each set's datastructure
	L1_CACHE_SET *set_i;
	for(int i=0; i < cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		// set[i] has "cache->n_rows" number of rows of type "L1_CACHE_ENTRY"
		set_i->row = (L1_CACHE_ENTRY*)malloc(sizeof(L1_CACHE_ENTRY) * cache->n_rows);

		// initialize each row's datastructure (in set[i])
		L1_CACHE_ENTRY *set_i_row_j;
		for(int j=0; j < cache->n_rows; ++j){
			set_i_row_j = &(set_i->row[j]);
			set_i_row_j->V = 0;
			set_i_row_j->D = 0;
			set_i_row_j->tag = 0;
			set_i_row_j->data = NULL;
		}
	}
}


L1_CACHE_ENTRY* L1_CACHE_read (L1_CACHE *cache, unsigned PADDR/*, void **read_data*/) 
{
	L1_CACHE_ENTRY *cache_response = L1_CACHE_lookup(cache, PADDR);
	return cache_response;
}


L1_CACHE_ENTRY* L1_CACHE_write(L1_CACHE *cache, unsigned PADDR/*, void **write_data*/) 
{
	L1_CACHE_ENTRY *cache_response = L1_CACHE_lookup(cache, PADDR);
	if(cache_response == MISS)
		return NULL;
	// if cache HIT!
	cache_response->D = 1;
	return cache_response;
}


L1_CACHE_ENTRY* L1_CACHE_fifo(L1_CACHE *cache,unsigned PADDR)
{
	unsigned int tag, index, offset;
	L1_CACHE_extract(PADDR, &tag, &index, &offset);

	L1_CACHE_SET *set_fifo = &(cache->set[ cache->fifo[index]->head->next->data ]);
	L1_CACHE_ENTRY *set_fifo_row_index = &(set_fifo->row[index]);

	return set_fifo_row_index;
}


L1_CACHE_ENTRY* L1_CACHE_insert(L1_CACHE *cache, unsigned PADDR)
{
	L1_CACHE_ENTRY *set_fifo_row_index = L1_CACHE_fifo(cache, PADDR);

	unsigned int tag, index, offset;
	L1_CACHE_extract(PADDR, &tag, &index, &offset);

	set_fifo_row_index->V = 1;
	set_fifo_row_index->D = 0;
	set_fifo_row_index->tag = tag;
	set_fifo_row_index->data = NULL;

	// update cache's fifo entry at given index
	QUEUE_ENTRY *head = QUEUE_remove(cache->fifo[index], 0); // remove head;
	QUEUE_insert(cache->fifo[index], head->data); // place at the tail
	return set_fifo_row_index;
}


L1_CACHE_ENTRY* L1_CACHE_remove(L1_CACHE *cache, unsigned PADDR)
{
	unsigned int tag, index, offset;
	L1_CACHE_extract(PADDR, &tag, &index, &offset);
	
	L1_CACHE_SET *set_i;
	L1_CACHE_ENTRY *set_i_row_index;

	int set_number = -1; // if hit {0,1,2,3}
	L1_CACHE_ENTRY *cache_response = MISS;
	for(int i=0; i<cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		set_i_row_index = &(set_i->row[index]);
		// tag-matching and validity-check
		if(set_i_row_index->tag == tag && set_i_row_index->V == 1){
			cache_response = HIT;
			set_number = i;
			break;
		}
	}

	if(cache_response == MISS)
		return NULL;
	// else
		// printf("c1tag: %d\n", PADDR);
	//if present int cache
	//find index of "set_number" in fifo
	int set_number_index = -1; 
	QUEUE_ENTRY* current = cache->fifo[index]->head->next;
	while(current!=NULL)
	{	
		set_number_index += 1;
		if(set_number == current->data){ break; }
		else { current = current->next; }
	}

	// update cache's fifo entry at given index
	QUEUE_ENTRY *set_entry = 
	QUEUE_remove(cache->fifo[index], set_number_index);
	QUEUE_finsert(cache->fifo[index], set_entry->data); // place at the head

	cache_response->V = 0; // invalidate
	return cache_response;
}


L1_CACHE_ENTRY* L1_CACHE_lookup(L1_CACHE *cache, unsigned PADDR)
{
	unsigned int tag, index, offset;
	L1_CACHE_extract(PADDR, &tag, &index, &offset);

	L1_CACHE_SET *set_i;
	L1_CACHE_ENTRY *set_i_row_index;
	for(int i=0; i<cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		set_i_row_index = &(set_i->row[index]);
		// tag-matching and validity-check
		if(set_i_row_index->tag == tag && set_i_row_index->V == 1)
			return HIT;
	}
	return MISS;
}


void L1_CACHE_show  (L1_CACHE *cache, int start_index, int end_index) 
{
	assert(start_index < end_index);
	assert(start_index >= 0);
	assert(end_index <= cache->n_rows);

	L1_CACHE_ENTRY set_i_row_j;
	for(int j=start_index; j < end_index; ++j)
	{
		QUEUE_show(cache->fifo[j]);
		for(int i=0; i < cache->n_sets; ++i)
		{
			set_i_row_j = cache->set[i].row[j]; 
			printf("s[%d]r[%3d][V:%d,D:%d,tag:%5d] | ",
				i,j,set_i_row_j.V, set_i_row_j.D,
				set_i_row_j.tag/*, set_i_row_j.data*/);
		}
		printf("\n");
	}
}

void L1_CACHE_extract(unsigned PADDR, uint* tag, uint* index, uint* offset)
{
	L1_extract extract;
	extract.PADDR = PADDR;

	*tag = extract.raw.tag;
	*index = extract.raw.index;
	*offset = extract.raw.offset;
}