#include <stdio.h>
#include <stdlib.h>

#include "tlb.h"

unsigned int min(int x, int y)
{
	if(x<y)
		return x;
	return y;
}

unsigned int max(int x, int y)
{
	if(x>y)
		return x;
	return y;
}

void bin(unsigned int N) 
{ 
    for (unsigned int i = 1 << 7; i > 0; i = i / 2) 
        (N & i)? printf("1"): printf("0"); 
    printf("\n");
}

void bin_string(char *s, unsigned int N, int digits)
{
	for(int i = 1; i<digits; i++)
		s[digits-i-1] = (N&(1<<(i-1)))?'1':'0';
	s[digits-1] = '\0';
}

void initialize_TLB(TLB *X, int N)
{
	X->TLB_size = N;
	X->entry = (TLB_ENTRY *)malloc(N*sizeof(TLB_ENTRY));
	X->entries_used = 0;
	X->hits = 0;
	X->misses = 0;

	for(int i = 0; i<N; i++)
	{
		X->entry[i].used = 0;
		X->entry[i].counter = 0;
	}
}

void show_TLB(TLB *X)
{
	// printf("#Entries in TLB: %d\n", X->TLB_size);

	char pid[4];
	char page_number[23];
	char frame_number[17];
	char valid[2];
	char used[2];
	char counter[6];

	for(int i = 0; i<X->TLB_size; i++)
	{
		bin_string(pid, X->entry[i].pid, 4);
		printf("PID: %s, ", pid);

		bin_string(page_number, X->entry[i].page_number, 23);
		printf("Page#: %s, ", page_number);

		bin_string(frame_number, X->entry[i].frame_number, 17);
		printf("Frame#: %s, ", frame_number);

		bin_string(valid, X->entry[i].valid, 2);
		printf("Valid?: %s, ", valid);

		bin_string(used, X->entry[i].used, 2);
		printf("Used?: %s, ", used);

		bin_string(counter, X->entry[i].counter, 6);
		printf("Counter: %s\n", counter);
	}
}

unsigned int get_TLB_slot(TLB *X)
{
	//find a slot which is unused
	for(int i = 0; i<X->TLB_size; i++)
		if(X->entry[i].used == 0)
			return i;

	//find a slot which is invalidated
	for(int i = 0; i<X->TLB_size; i++)
		if(X->entry[i].valid == 0)
			return i;

	//find a slot according to LRU replacement policy
	for(int i = 0; i<X->TLB_size; i++)
		if(X->entry[i].counter == 0)
			return i;

	//not possible
	return X->TLB_size;
}

void update_TLB_counters(TLB *X, unsigned int ENTRY_NUMBER, unsigned int HIT)
{
	// if(X->entries_used != X->TLB_size)
	// {
	// 	if(HIT == 0)
	// 		X->entry[ENTRY_NUMBER].counter = X->entries_used;
	// 	else
	// 	{
	// 		for(int i = 0; i<X->entries_used; i++)
	// 		{
	// 			if(i == ENTRY_NUMBER)
	// 				continue;
	// 			if(X->entry[i].counter > X->entry[ENTRY_NUMBER].counter)
	// 				X->entry[i].counter = X->entry[i].counter-1;
	// 		}
	// 		X->entry[ENTRY_NUMBER].counter = X->entries_used-1;
	// 	}
	// }
	// else
	// {
	// 	for(int i = 0; i<X->TLB_size; i++)
	// 	{
	// 		if(i == ENTRY_NUMBER)
	// 			continue;
	// 		if(X->entry[i].counter > X->entry[ENTRY_NUMBER].counter)
	// 			X->entry[i].counter = X->entry[i].counter-1;
	// 	}
	// 	X->entry[ENTRY_NUMBER].counter = X->TLB_size-1;
	// }


	if(X->entries_used < X->TLB_size && HIT == 0)
	{
		X->entry[ENTRY_NUMBER].counter = X->entries_used;
		return;
	}
	for(int i = 0; i<X->entries_used; i++)
	{
		if(i == ENTRY_NUMBER)
			continue;
		if(X->entry[i].counter > X->entry[ENTRY_NUMBER].counter)
			X->entry[i].counter = X->entry[i].counter-1;
	}
	if(X->entries_used == X->TLB_size && HIT == 0)
		X->entry[ENTRY_NUMBER].counter = X->TLB_size-1;
	X->entry[ENTRY_NUMBER].counter = min(X->entries_used-1, X->TLB_size-1);
}

void update_TLB(TLB *X, unsigned int ENTRY_NUMBER, unsigned int PAGE_NUMBER, unsigned int FRAME_NUMBER, unsigned int PID, unsigned int HIT)
{
	update_TLB_counters(X, ENTRY_NUMBER, HIT);
	if(HIT == 0)
	{	
		X->entry[ENTRY_NUMBER].pid = PID;
		X->entry[ENTRY_NUMBER].page_number = PAGE_NUMBER;
		X->entry[ENTRY_NUMBER].frame_number = FRAME_NUMBER;
		X->entry[ENTRY_NUMBER].valid = 1;
		X->entry[ENTRY_NUMBER].used = 1;	
		X->entries_used = min(X->entries_used+1, X->TLB_size);
	}
}

void invalidate_process_TLB(TLB *X, unsigned int PID)
{
	// process has terminated: make all it's TLB entries invalid
	for(int i = 0; i<X->TLB_size; i++)
		if(X->entry[i].pid == PID)
			X->entry[i].valid = 0;
}

unsigned int find(TLB *X, unsigned int LADDR, unsigned int PID, char TYPE)
{
	int TLB_HIT = 0;
	int FRAME_NUMBER, PAGE_NUMBER, BYTE_NUMBER, ENTRY_NUMBER;

	PAGE_NUMBER = LADDR>>10;

	for(int i = 0; i<X->TLB_size; i++)
	{
		if(X->entry[i].valid == 0)
			continue;
		if(PID == X->entry[i].pid && PAGE_NUMBER == X->entry[i].page_number)
		{
			TLB_HIT = 1;
			ENTRY_NUMBER = i;
			FRAME_NUMBER = X->entry[i].frame_number;
			break;
		}
	}

	if(TLB_HIT)
	{
		if(TYPE != 'X')
			update_TLB(X, ENTRY_NUMBER, PAGE_NUMBER, FRAME_NUMBER, PID, TLB_HIT);
		else
		{
			printf("TLB HIT!\n");
			X->hits++;
		}
	}
	else
	{
		printf("TLB MISS!\n");
		X->misses++;
		// printf("Starting Address Translation!\n");

		// printf("Address Translation Complete!\n");
		printf("Updating TLB!\n");

		// ENTRY_NUMBER = get_TLB_slot(X);

		// update_TLB(X, ENTRY_NUMBER, PAGE_NUMBER, FRAME_NUMBER, PID, TLB_HIT);
	}

	BYTE_NUMBER = FRAME_NUMBER<<10 + (LADDR&((1<<10)-1));

	// hit or miss
	if(TYPE == 'X')
		return TLB_HIT;

	// frame#
	if(TYPE == 'F')
		return FRAME_NUMBER;

	// byte#
	if(TYPE == 'B')
		return BYTE_NUMBER;

	//invalid type
	return -1;
}

// unsigned int resolve(unsigned int LADDR)
// {
// 	unsigned int seg = (LADDR>>29);
// 	unsigned int pt1 = (LADDR>>26)&((1<<3) - 1);
// 	unsigned int pt2 = (LADDR>>18)&((1<<8) - 1);
// 	unsigned int pt3 = (LADDR>>10)&((1<<8) - 1);
// 	unsigned int off = (LADDR>>0)&((1<<10) - 1);

// 	// printf("%u %u %u %u %u\n", seg, pt1, pt2, pt3, off);

// 	// bin(seg);
// 	// bin(pt1);
// 	// bin(pt2);
// 	// bin(pt3);
// 	// bin(off);


// 	unsigned int FRAME_NUMBER;
	
// 	//  	will require memory, segment tables and page 
// 	// 	tables to be done first
	

// 	// seg_descriptor = segment_table[seg];

// 	// pt1_table = memory[seg_descriptor->base];
// 	// pt1_entry = pt1_table[pt1];
	
// 	// p2_table = memory[pt1_entry->data];
// 	// pt2_entry = pt2_table[pt2];

// 	// pt3_table = memory[pt2_entry->data];
// 	// pt3_entry = pt3_table[pt3];
	
// 	// FRAME_NUMBER = pt3_entry->data;

// 	// reqd_byte = memory[FRAME_NUMBER<<10 + off];
	

// 	FRAME_NUMBER = (pt2<<8)+pt3;
// 	return FRAME_NUMBER;
// }
