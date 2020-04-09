#ifndef PAGETABLE_H
#define PAGETABLE_H

#define NUM_PAGETABLES 256*8+8+1
#define PAGETABLE_SIZE 256

typedef struct
{
	unsigned int frame:16;
	unsigned int valid:1;
} PAGEENTRY;


typedef struct
{
	unsigned int base:16; 
	PAGEENTRY **entry;
	unsigned int pagetable_size;
	unsigned int num_pagetables;
} PAGETABLE;


void initialize_pagetable(PAGETABLE *X, unsigned int NUM_TABLES, unsigned int N);

unsigned int get_nextaddress(MEMORY *Y,L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, PAGETABLE *X, unsigned int PID, unsigned int BASE, unsigned int OFFSET, int FINAL);
unsigned int get_frame_address(MEMORY *Y, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, PAGETABLE *X, unsigned int PID, unsigned int PTBR, unsigned int OFFSET);

void print_pagetable(PAGETABLE *X);

#endif