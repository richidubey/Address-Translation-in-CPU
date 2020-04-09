#ifndef MEMORY_H
#define MEMORY_H

#include "../cache/L1_CACHE.h"
#include "../cache/L2_CACHE.h"
#include "../cache/write_buffer.h"

#define MEMORY_SIZE 65536

typedef struct
{
	unsigned int valid:1;
	unsigned int dirty:1;
	unsigned int owner:3;
	unsigned int refer:1;
} FRAME;

extern int lastfifo;

typedef struct
{
	FRAME *frame;
	unsigned int memory_size;
} MEMORY;

void initialize_memory(MEMORY *X, int N);

int find_free_frame(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID);

int load_memory(MEMORY *X, int PADDR, unsigned int PID);

int replace_sec_chance(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID);
int read_memory(MEMORY *X, int PADDR, unsigned int PID);
int process_dies(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID);
int write_memory(MEMORY *X, int PADDR, unsigned int PID);
#endif
