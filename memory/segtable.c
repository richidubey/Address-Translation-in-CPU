#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "segtable.h"

#define print_in_binary_pattern "%c%c%c%c%c%c%c%c"
#define to_bin(byte)	(byte & 0x80 ? '1' :'0'), (byte & 0x40 ? '1' :'0'), (byte & 0x20 ? '1' :'0'), (byte & 0x10 ? '1' :'0'), (byte & 0x08 ? '1' :'0'), (byte & 0x04 ? '1' :'0'), (byte & 0x02 ? '1' :'0'), (byte & 0x01 ? '1' :'0')

void initialize_segtable(SEGTABLE *X, int N)
{
	X->segtable_size = N;
	X->segentry = (SEGENTRY *)malloc(N*sizeof(SEGENTRY));

	for(int i = 0; i<N; i++)
	{
		// X->segentry[i].base =  
		X->segentry[i].limit = ((1<<29)-1);
		X->segentry[i].valid=0;
	}	
}

unsigned int get_page_table_address(MEMORY *Y, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, SEGTABLE *X, unsigned int PID, unsigned int SEGNUM)
{
	
	printf("Inside Segment Table : Finding Outer Page Table Base Address for Process %d and Segment %d\n",PID,SEGNUM);
	
	if(X->segentry[SEGNUM].valid==1)
	{
		if(read_memory(Y,X->segentry[SEGNUM].base,PID)!=0)
			return X->segentry[SEGNUM].base;
		//TODO: Send and check limit value as well.
		
	}
	
	int PADDR=find_free_frame(Y,L1,L2,wb,PID);
	
	if(PADDR==-1)
	{
		printf("No Memory Available! Error. FIFO not implemented!!");//TODO
		exit(0);
	}
	
	int ret=load_memory(Y, PADDR, PID);
	
	if(ret==-1)
	{
		printf("Error! Invalid Address Reference or permission");
		exit(0);
	}
	
	printf("Loaded OuterMost Page Table in MM for process %d at location : %d(" print_in_binary_pattern" " print_in_binary_pattern")\n",PID,PADDR,to_bin((PADDR>>8)),to_bin(PADDR));
	
	X->segentry[SEGNUM].base=PADDR;
	X->segentry[SEGNUM].valid=1;
	// X->segentry[SEGNUM].limit=0b1111111111111111;
	
	return X->segentry[SEGNUM].base;
	
}

void print_table(SEGTABLE *X, int PID)
{

	printf("Segment Table (Process %d):\n Base(LSB 8)\tLimit(MSB 8)\n",PID);
	
	for(int j=0;j<X->segtable_size;j++)
		printf("  " print_in_binary_pattern "\t "  print_in_binary_pattern "\n",to_bin(X->segentry[j].base),to_bin((X->segentry[j].limit) >> 21));
		//For printing limit, we are only printing the last 8 bytes, i.e. 29-8 = From 21st byte, so right shift by 21 bits

	printf("\n\n");

}
