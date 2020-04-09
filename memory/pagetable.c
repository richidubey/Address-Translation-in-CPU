//Outer Page Table should stay in memory. 
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "pagetable.h"

#define print_in_binary_pattern "%c%c%c%c%c%c%c%c"
#define to_bin(byte)	(byte & 0x80 ? '1' :'0'), (byte & 0x40 ? '1' :'0'), (byte & 0x20 ? '1' :'0'), (byte & 0x10 ? '1' :'0'), (byte & 0x08 ? '1' :'0'), (byte & 0x04 ? '1' :'0'), (byte & 0x02 ? '1' :'0'), (byte & 0x01 ? '1' :'0')

void initialize_pagetable(PAGETABLE *X, unsigned int NUM_TABLES, unsigned int N)
{
	X->pagetable_size = N;
	X->num_pagetables = NUM_TABLES;
	X->entry = (PAGEENTRY **)malloc(NUM_TABLES*sizeof(PAGEENTRY *));
	for(int i = 0; i<NUM_TABLES; i++)
	{
		X->entry[i] = (PAGEENTRY *)malloc(N*sizeof(PAGEENTRY));	
		for(int j = 0; j<N; j++)
		{
			X->entry[i][j].valid = 0;
		}
	}
}

unsigned int get_nextaddress(MEMORY *Y,L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, PAGETABLE *X, unsigned int PID, unsigned int BASE, unsigned int OFFSET, int FINAL)
{	

	printf("Requested for %d OFFSET at Page Table BASE %d\n",OFFSET,BASE);
	
	
	if(X->entry[BASE][OFFSET].valid==1)
	{
		printf("Offset %d valid, retrieving content\n",OFFSET);
		if(read_memory(Y, X->entry[BASE][OFFSET].frame,PID)!=0)
		{
			// printf("erroreorarfo\n");
			return X->entry[BASE][OFFSET].frame; 
		}
	}
		if(FINAL==0)
		printf("Finding a frame in Main Memory to load the Page Table present at offset %d\n",OFFSET);
		
		else printf("Finding a frame in Main Memory to load the Data present at offset %d\n",OFFSET);
	
		int add=find_free_frame(Y,L1,L2,wb,PID);		
			
		if(add==-1)
		{
			printf("Error getting free location from memory! Abort");
			exit(0);
		}
		
		load_memory(Y,add,PID);
		
		if(FINAL==0)
			printf("Loading Page Table at location " print_in_binary_pattern " " print_in_binary_pattern " in MM\n",to_bin((add>>8)),to_bin(add));
		
		else
			printf("Loaded physical data into main memory at location = %d!\n", add);
		
		X->entry[BASE][OFFSET].frame=add;
		X->entry[BASE][OFFSET].valid=1;
		
		
		return X->entry[BASE][OFFSET].frame; 
}

unsigned int get_frame_address(MEMORY *Y, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, PAGETABLE *X, unsigned int PID, unsigned int PTBR, unsigned int OFFSET)
{
	
	unsigned int pass=get_nextaddress(Y,L1,L2,wb,X,PID,PTBR,OFFSET>>16,0);
	
	pass=get_nextaddress(Y,L1,L2,wb,X,PID,pass,((OFFSET>>8)&0b00011111111),0);
	
	return get_nextaddress(Y,L1,L2,wb,X,PID,pass,OFFSET&0b11111111,1);
}

void print_page_table(PAGETABLE *X)
{
	// printf("Page Table Base: "print_in_binary_pattern  " " print_in_binary_pattern "\n" ,to_bin((X->base>> 8)), to_bin(X->base));
	// for(int i=0;i<X->num_pagetables;i++)
	// {	
	// 	printf(print_in_binary_pattern " " print_in_binary_pattern "\t %d\n",to_bin((X->entry[i].frame>> 8)),to_bin((pt.entry[i].frame)),pt.entry[i].valid);
	// }
}
