#ifndef L2_CACHE_H
#define L2_CACHE_H

/* REFER TO "assoc.png" for basic design & 
"question.png" for additional requirements*/

typedef struct L2_CACHE_ENTRY
{
	unsigned int V : 1; // VALID-BIT
	unsigned int D : 1; // DIRTY-BIT
	unsigned int tag : 15; // scratchpad.txt // MAX = 32768
	// counter updation 0000->...->1111->0000->...
	unsigned int counter : 4; // question.txt 
	void* data;
} L2_CACHE_ENTRY;

typedef struct L2_CACHE_SET{
	L2_CACHE_ENTRY *row;
} L2_CACHE_SET; 

typedef struct L2_CACHE{
	L2_CACHE_SET *set;
	int n_sets; // set to 16 (question.png)
	int n_rows; // set to 64 (scratchpad.txt)
	int hits;
	int misses;
} L2_CACHE;

typedef union {
	/*PADDR format (assume 32-bit address space) (address.png)
	000000 <TAG [25..11] > <INDEX [10..4] > <BLOCK_OFFSET [3..0] >
	*/
	unsigned PADDR;
	struct 
	{
		unsigned int offset : 5;
		unsigned int index : 6;
		unsigned int tag : 21;
	} raw;
   
} L2_extract;

/************************************************************/

void L2_CACHE_init(L2_CACHE *cache/*, int n_sets, int n_rows*/);
void L2_CACHE_show(L2_CACHE *cache, int start_index, int end_index);
void L2_CACHE_extract(unsigned PADDR, unsigned *tag, unsigned *index, unsigned *offset);
L2_CACHE_ENTRY* L2_CACHE_read (L2_CACHE *cache, unsigned PADDR/*, void **read_data*/);
L2_CACHE_ENTRY* L2_CACHE_write(L2_CACHE *cache, unsigned PADDR/*, void **write_data*/);
L2_CACHE_ENTRY* L2_CACHE_insert(L2_CACHE *cache, unsigned PADDR);
L2_CACHE_ENTRY* L2_CACHE_remove(L2_CACHE *cache, unsigned PADDR);
L2_CACHE_ENTRY* L2_CACHE_lookup(L2_CACHE *cache, unsigned PADDR);
L2_CACHE_ENTRY* L2_CACHE_lru(L2_CACHE *cache, unsigned PADDR);

/************************************************************/

#endif