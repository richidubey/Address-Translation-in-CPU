#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h> 
#include "L2_CACHE.h"

#define HIT set_i_row_index
#define MISS NULL

void L2_CACHE_counter_update(L2_CACHE *cache, L2_CACHE_ENTRY* cache_response, unsigned PADDR);

void L2_CACHE_init  (L2_CACHE *cache/*, int n_sets, int n_rows*/)
{
	cache->n_sets = 16; // n_sets;
	cache->n_rows = 64; // n_rows;
	cache->set = (L2_CACHE_SET*)malloc(sizeof(L2_CACHE_SET) * cache->n_sets);
	cache->hits = 0;
	cache->misses = 0;
	// initialize each set's datastructure
	L2_CACHE_SET *set_i;
	for(int i=0; i < cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		// set[i] has "cache->n_rows" number of rows of type "L1_CACHE_ENTRY"
		set_i->row = (L2_CACHE_ENTRY*)malloc(sizeof(L2_CACHE_ENTRY) * cache->n_rows);

		// initialize each row's datastructure (in set[i])
		L2_CACHE_ENTRY *set_i_row_j;
		for(int j=0; j < cache->n_rows; ++j){
			set_i_row_j = &(set_i->row[j]);
			set_i_row_j->V = 0;
			set_i_row_j->D = 0;
			set_i_row_j->tag = 0;
			set_i_row_j->counter = 0;
			set_i_row_j->data = NULL;
		}
	}
}


L2_CACHE_ENTRY* L2_CACHE_read (L2_CACHE *cache, unsigned PADDR/*, void **read_data*/) 
{
	L2_CACHE_ENTRY *cache_response = L2_CACHE_lookup(cache, PADDR);
	if(cache_response == MISS)
		return NULL;
	// if cache HIT!
	L2_CACHE_counter_update(cache, cache_response, PADDR);
	return cache_response;
}


L2_CACHE_ENTRY* L2_CACHE_write(L2_CACHE *cache, unsigned PADDR/*, void **write_data*/) 
{
	L2_CACHE_ENTRY *cache_response = L2_CACHE_lookup(cache, PADDR);
	if(cache_response == MISS)
		return NULL;
	// if cache HIT!
	L2_CACHE_counter_update(cache, cache_response, PADDR);
	cache_response->D = 1;
	return cache_response;
}


L2_CACHE_ENTRY* L2_CACHE_lookup(L2_CACHE *cache, unsigned PADDR)
{
	unsigned int tag, index, offset;
	L2_CACHE_extract(PADDR, &tag, &index, &offset);

	L2_CACHE_SET *set_i;
	L2_CACHE_ENTRY *set_i_row_index;
	for(int i=0; i<cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		set_i_row_index = &(set_i->row[index]);
		// tag-matching and validity-check
		if(set_i_row_index->tag == tag && set_i_row_index->V == 1)
			return HIT;
	}
	return MISS;
}


void L2_CACHE_show  (L2_CACHE *cache, int start_index, int end_index) 
{
	assert(start_index < end_index);
	assert(start_index >= 0);
	assert(end_index <= cache->n_rows);

	L2_CACHE_ENTRY set_i_row_j;
	for(int j=start_index; j < end_index; ++j)
	{
		for(int i=0; i < cache->n_sets; ++i)
		{
			set_i_row_j = cache->set[i].row[j]; 
			// printf("cnt=%2d |",set_i_row_j.counter);
			printf("s[%2d]r[%2d][V:%d,D:%d,tag:%5d,cnt=%2d] | ",
				i,j,set_i_row_j.V, set_i_row_j.D,
				set_i_row_j.tag, set_i_row_j.counter
				/*, set_i_row_j.data*/);
		}
		printf("\n");
	}
}


void L2_CACHE_counter_update(L2_CACHE *cache, L2_CACHE_ENTRY* cache_response, unsigned PADDR)
{
	unsigned int tag, index, offset;
	L2_CACHE_extract(PADDR, &tag, &index, &offset);

	L2_CACHE_SET *set_i;
	L2_CACHE_ENTRY *set_i_row_index;
	for(int i=0; i<cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		set_i_row_index = &(set_i->row[index]);

		if(set_i_row_index->counter > cache_response->counter)
			(set_i_row_index->counter)--;
	}
	cache_response->counter = (cache->n_sets)-1; // set to 15 = 1111b
}


L2_CACHE_ENTRY* L2_CACHE_lru(L2_CACHE *cache, unsigned PADDR)
{
	unsigned int tag, index, offset;
	L2_CACHE_extract(PADDR, &tag, &index, &offset);

	L2_CACHE_SET *set_lru;
	L2_CACHE_ENTRY *set_lru_row_index;

	int macache_counter = 100; // set counter not greater than 15!
	L2_CACHE_SET *set_i;
	L2_CACHE_ENTRY *set_i_row_index;
	for(int i=0; i<cache->n_sets; ++i){
		set_i = &(cache->set[i]);
		set_i_row_index = &(set_i->row[index]);
		// invalid entries MOST prefered 
		if(set_i_row_index->V == 0)
		{
			set_lru = set_i;
			break;
		}
		// valid entries, least counter preferred
		else if(set_i_row_index->counter < macache_counter)
		{
			set_lru = set_i;
			macache_counter = set_i_row_index->counter;
		}
	}

	set_lru_row_index = &(set_lru->row[index]);
	return set_lru_row_index;
}

L2_CACHE_ENTRY* L2_CACHE_insert(L2_CACHE *cache, unsigned PADDR)
{
	L2_CACHE_ENTRY *set_lru_row_index = L2_CACHE_lru(cache, PADDR);

	unsigned int tag, index, offset;
	L2_CACHE_extract(PADDR, &tag, &index, &offset);

	set_lru_row_index->V = 1;
	set_lru_row_index->D = 0;
	set_lru_row_index->tag = tag;
	set_lru_row_index->data = NULL;
	
	L2_CACHE_counter_update(cache, set_lru_row_index, PADDR);
	return set_lru_row_index;
}


L2_CACHE_ENTRY* L2_CACHE_remove(L2_CACHE *cache, unsigned PADDR)
{
	L2_CACHE_ENTRY *cache_response = L2_CACHE_lookup(cache, PADDR);
	if(cache_response == MISS)
		return NULL;
	// else
		// printf("ctag2: %d\n", PADDR);
	//if present int cache
	cache_response->V = 0; // invalidate
	cache_response->counter = 0;
	return cache_response;
}


void L2_CACHE_extract(unsigned PADDR, uint *tag, uint *index, uint *offset)
{
	L2_extract extract;
	extract.PADDR = PADDR;

	*tag = extract.raw.tag;
	*index = extract.raw.index;
	*offset = extract.raw.offset;
}