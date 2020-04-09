#ifndef SEGTABLE_H
#define SEGTABLE_H

#define SEGTABLE_SIZE 4

typedef struct 
{
    unsigned int base:26;
    unsigned int limit:29;
    unsigned int valid:1;
    
} SEGENTRY;

typedef struct
{
	SEGENTRY *segentry;
	unsigned int segtable_size;
} SEGTABLE;

void initialize_segtable(SEGTABLE *X, int N);

unsigned int get_page_table_address(MEMORY *Y, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, SEGTABLE *X, unsigned int PID, unsigned int SEGNUM);

void print_table(SEGTABLE *X, int PID);

#endif