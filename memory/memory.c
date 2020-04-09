//TODO : Max limit for number of frames for a process.


#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "../cache/cache_controller.h"
#include "../scheduler/scheduler.h"

#define print_in_binary_pattern "%c%c%c%c%c%c%c%c"
#define to_bin(byte)	(byte & 0x80 ? '1' :'0'), (byte & 0x40 ? '1' :'0'), (byte & 0x20 ? '1' :'0'), (byte & 0x10 ? '1' :'0'), (byte & 0x08 ? '1' :'0'), (byte & 0x04 ? '1' :'0'), (byte & 0x02 ? '1' :'0'), (byte & 0x01 ? '1' :'0')

	
int lastfifo;
void initialize_memory(MEMORY *X, int N)
{	
	lastfifo=MAX_PROCESSES;
	X->memory_size = N;
	X->frame = (FRAME *)malloc(N*sizeof(FRAME));

	for(int i = 0; i<N; i++)
	{
		X->frame[i].valid = 0;
		X->frame[i].owner = 0;
		X->frame[i].dirty=0;
		X->frame[i].refer=0;
	}
}
	
int replace_sec_chance(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID)
{
	printf("Memory Full!! Finding a frame to replace.\n");
	int chosen=-1;
	
	for(int i=lastfifo;i<X->memory_size+lastfifo;i++)
	{
	
		int ni=i%(X->memory_size);
		
		if(X->frame[ni].refer==1)
		{
			X->frame[ni].refer=0;
		}
		else
		{
			if(X->frame[ni].dirty==0)
			{
				chosen=ni;
				lastfifo=ni+1;
				break;
			}
		}
	}	
	

	
	if(chosen==-1)	//Checking if we had some frame which earlier had reference bit =1 and was non dirty
	{
		for(int i=lastfifo;i<X->memory_size+lastfifo;i++)
		{
			int ni=i%(X->memory_size);
				if(X->frame[ni].dirty==0)
				{
					chosen=ni;
					lastfifo=ni+1;
					break;
				}
		}	
	}
	
	
	
	if(chosen==-1)	//Finding first dirty page and replacing it.
	{
		for(int i=lastfifo;i<X->memory_size+lastfifo;i++)
		{
				int ni=i%(X->memory_size);
				
				if(X->frame[ni].dirty==0)
				{
					chosen=ni;
					lastfifo=ni+1;
					break;
				}
				
				else if(X->frame[ni].dirty==1)
				{
					chosen=ni;
					lastfifo=ni+1;
					
					break;
				}
		}	
	}
	
	if(lastfifo<MAX_PROCESSES)
		lastfifo=MAX_PROCESSES;
	// printf("mtag s chance: %d\n", chosen);
	invalidate_cache(X,L1,L2,wb,chosen,PID);
	
	return chosen;
}

int find_free_frame(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID)
{
	printf("Finding a free frame in main memory \n");
	for(int i=0;i<X->memory_size;i++)
		if(X->frame[i].valid==0)
			return i;	
	
	
	return replace_sec_chance(X,L1,L2,wb,PID);
}


int read_memory(MEMORY *X, int PADDR, unsigned int PID)
{

	if(PADDR<0 || PADDR >= X->memory_size )
		return -1;
	
	if(PID!=X->frame[PADDR].owner||X->frame[PADDR].valid==0)
	{
		if(PID!=X->frame[PADDR].owner)
		printf("Error in reading, the frame address : %d (" print_in_binary_pattern " " print_in_binary_pattern  ") belongs to %d and PID of requesting process is : %d \n",PADDR,to_bin((PADDR>>8)),to_bin(PADDR),X->frame[PADDR].owner, PID);
		
		else printf("Frame %d is not valid - its process might have died\n",PADDR); //Error: Shouldn't happen!! 
		
		
	return 0;	// 0 indicates page does not belong to the calling process
		//TODO : Check that this never happens!!
	}
	
	
	X->frame[PADDR].refer=1;
	
	printf("Read frame : %d (" print_in_binary_pattern " " print_in_binary_pattern ") in Main Memory\n",PADDR,to_bin((PADDR>>8)),to_bin(PADDR)); 
	return 1;
}


int process_dies(MEMORY *X, L1_CACHE *L1, L2_CACHE *L2, write_buffer *wb, unsigned int PID)
{
	for(int i=MAX_PROCESSES;i<X->memory_size;i++)
	{
		if(X->frame[i].owner==PID)
		{
			X->frame[i].valid=0;
			
			if(X->frame[i].dirty==1)
			{
				printf("Memory Location : %d (" print_in_binary_pattern " " print_in_binary_pattern "), belonging to %d written back to disk\n",i,to_bin((i>>8)),to_bin(i),PID);
			}
			X->frame[i].dirty=0;
			X->frame[i].refer=0;
			// printf("mtag p dies: %d\n", i);
			invalidate_cache(X,L1,L2,wb,i,PID);
	
		}
	}
	return -1;
}

 
int write_memory(MEMORY *X, int PADDR, unsigned int PID)
{

	if(PADDR<0 || PADDR >= X->memory_size)
		return -1;
		
		
	if(PID!=X->frame[PADDR].owner)
	{
		// printf("Error in writing, the frame address : %d belongs to %d\n",PADDR,X->frame[PADDR].owner);
		return 0;	// 0 indicates page does not belong to the calling process
		//TODO : Check that this never happens!!
	}
	
	
	X->frame[PADDR].dirty=1;
	X->frame[PADDR].refer=1;
	
	
	return 1;
}

int load_memory(MEMORY *X, int PADDR, unsigned int PID)	//PADDR is only till frame, not offset
{
	
	if(PADDR<0 || PADDR >= X->memory_size)
		return -1;

	X->frame[PADDR].valid=1;
	X->frame[PADDR].owner=PID;
	X->frame[PADDR].dirty=0;
	X->frame[PADDR].refer=0;
	
	return 1;
}
