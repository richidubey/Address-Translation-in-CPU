#ifndef L1_CACHE_H
#define L1_CACHE_H

#include "./FIFO/fifo.h"

/* REFER TO "assoc.png" for basic design & 
"question.png" for additional requirements*/

typedef struct L1_CACHE_ENTRY
{
	unsigned int V : 1; // VALID-BIT
	unsigned int D : 1; // DIRTY-BIT
	unsigned int tag : 15; // scratchpad.txt // MAX = 32768
	void* data;
} L1_CACHE_ENTRY;

typedef struct L1_CACHE_SET{
	L1_CACHE_ENTRY *row;
} L1_CACHE_SET; 

typedef struct L1_CACHE{
	L1_CACHE_SET *set;
	int n_sets; // set to 4 (question.png)
	int n_rows; // set to 128 (scratchpad.txt)
	int hits;
	int misses;
	// block replacement policy (question.png)
	/* fifo[j].head = next set in "row j" 
	whose block is replaced*/
	QUEUE **fifo; // maintains fifo order (for each row)
} L1_CACHE;

typedef union {
	/*PADDR format (assume 32-bit address space) (address.png)
	000000 <TAG [25..11] > <INDEX [10..4] > <BLOCK_OFFSET [3..0] >
	*/
	unsigned PADDR;
	struct 
	{
		unsigned int offset : 4;
		unsigned int index : 7;
		unsigned int tag : 21;
	} raw;
   
} L1_extract;

/************************************************************/

void L1_CACHE_init(L1_CACHE *cache/*, int n_sets, int n_rows*/);
void L1_CACHE_show(L1_CACHE *cache, int start_index, int end_index);
void L1_CACHE_extract(unsigned PADDR, unsigned *tag, unsigned *index, unsigned *offset);
L1_CACHE_ENTRY* L1_CACHE_read (L1_CACHE *cache, unsigned PADDR/*, void **read_data*/);
L1_CACHE_ENTRY* L1_CACHE_write(L1_CACHE *cache, unsigned PADDR/*, void **write_data*/);
L1_CACHE_ENTRY* L1_CACHE_insert(L1_CACHE *cache, unsigned PADDR);
L1_CACHE_ENTRY* L1_CACHE_remove(L1_CACHE *cache, unsigned PADDR);
L1_CACHE_ENTRY* L1_CACHE_lookup(L1_CACHE *cache, unsigned PADDR);
L1_CACHE_ENTRY* L1_CACHE_fifo(L1_CACHE *cache, unsigned PADDR);

/************************************************************/

#endif