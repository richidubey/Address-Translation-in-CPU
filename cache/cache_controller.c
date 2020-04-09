#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache_controller.h"

/*

things to keep note of later
using functions like bin and bin string from tlb
remove while making final code


whenever using bin_string string array of one bigger
size

Note:

imp things about cache controller
write - this bit is 1 if it's a write operation else read

*/



// void bin(unsigned int N) 
// { 
//     for (unsigned int i = 1 << 7; i > 0; i = i / 2) 
//         (N & i)? printf("1"): printf("0"); 
//     printf("\n");
// }

// void bin_string(char *s, unsigned int N, int digits)
// {
// 	for(int i = 1; i<digits; i++)
// 		s[digits-i-1] = (N&(1<<(i-1)))?'1':'0';
// 	s[digits-1] = '\0';
// }


void invalidate_cache(MEMORY *X,L1_CACHE*cache1, L2_CACHE*cache2,write_buffer *wb, unsigned PADDR,unsigned int PID)
{
	PADDR=PADDR<<10;
	char addr[27];
	bin_string(addr,PADDR,27); 
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<2;j++)
		{
			for(int k=0;k<2;k++)
			{
				for(int l=0;l<2;l++)
				{
					for(int m=0;m<2;m++)
					{
						addr[16]='0'+i;
						addr[17]='0'+j;
						addr[18]='0'+k;
						addr[19]='0'+l;
						addr[20]='0'+m;

						addr[21]='0';
 
						//first invalidate in L1						
						//then put from WB to L2

						unsigned temp;
						temp=convert_to_PADDR(addr);

						invalidate_in_L1(cache1,temp);
						delete_update(wb,addr,cache2);	
 
						addr[21]='1';
						//same
						temp=convert_to_PADDR(addr);						
						invalidate_in_L1(cache1,temp);
						delete_update(wb,addr,cache2);
						
						//after both!
						//then put from L2 to MM
						L2_CACHE_ENTRY* rem1=L2_CACHE_lookup(cache2,PADDR);
						if(rem1 && rem1->D!=0)
							write_memory(X,(PADDR>>10),PID);
						
						//then invalidate in L2
						L2_CACHE_ENTRY* rem=L2_CACHE_remove(cache2,PADDR);
					}
				}
			}
		}
	}
 
}

void load_to_L1(L1_CACHE *cache1,L2_CACHE* cache2,write_buffer *wb ,unsigned PADDR)
{
	//find target ==> the entry which i am going to replace
	L1_CACHE_ENTRY *entry1=L1_CACHE_fifo(cache1,PADDR);
	
	if(entry1->V!=1)
	{
		L1_CACHE_insert(cache1,PADDR);
		return;
	}

	L1_extract extract;
	extract.PADDR = PADDR;
	unsigned int index;
	index = extract.raw.index;

	//L1_replacer -> do the necessary
	L1_replacer(cache1,cache2,entry1,wb,index);
	L1_CACHE_insert(cache1,PADDR);
}

void load_to_L2(MEMORY *X,L1_CACHE *cache1,L2_CACHE* cache2,write_buffer *wb ,unsigned PADDR,unsigned PID)
{
	L2_CACHE_ENTRY *entry2= L2_CACHE_lru(cache2,PADDR);
	if(entry2->V==0)
	{
		L2_CACHE_insert(cache2,PADDR);
		return;
	}

	L2_extract extract;
	extract.PADDR = PADDR;
	unsigned int index;
	index = extract.raw.index;

	L2_replacer(X,cache1,cache2,entry2,wb,index,PID);
	L2_CACHE_insert(cache2,PADDR);
}


void delete_update(write_buffer* wb,char* addr,L2_CACHE *cache2)
{
	int index=search_write_buffer(wb,addr,21);

	//if there update to L2
	if(index!=-1)
		update_buffer_to_L2(index,wb,cache2);
}
void L1_replacer(L1_CACHE *cache1,L2_CACHE* cache2,L1_CACHE_ENTRY *entry,write_buffer * wb,unsigned index)
{
	//will be called whenever there is a ***replacement*** in L1

	//function argument - 
	//1)the entry which is going to be replaced!
	//2)index
	// 0-14 bit are tag 
	// 15-21 are index
	//isse make tag,index bit ka string


	unsigned tag=entry->tag;
	char addr[27];
	bzero(addr,27);
	int i,j;
	for(i=0;i<=14;i++)
	{
		if(((tag)&(1<<i))==0)
			addr[14-i]='0';
		else	
			addr[14-i]='1';
	}

	for(i=0,j=15;i<=6;i++,j++)
	{
		if(((index)&(1<<i))==0)
			addr[21-i]='0';
		else	
			addr[21-i]='1';
	}
	
	delete_update(wb,addr,cache2);
	//return back to L1_insert where it will handle replacement
}
//L2 tag 15 index 6 offset 5
//L1 tag 15 index 7 offset 4


void invalidate_in_L1(L1_CACHE *cache1,unsigned PADDR)
{
	// L1_CACHE_ENTRY* presL1=L1_CACHE_lookup(cache1,PADDR);
	// presL1->V=0;
	L1_CACHE_remove(cache1,PADDR);
}
void L2_replacer(MEMORY *X, L1_CACHE *cache1,L2_CACHE *cache2,L2_CACHE_ENTRY *entry2,write_buffer* wb,unsigned index,unsigned PID)
{
	//get the entry which is going to be replaced
	unsigned tag=entry2->tag;
	char addr[27];
	bzero(addr,27);
	int i,j;
	for(i=0;i<=25;i++)
		addr[i]='0';
	for(i=0;i<=14;i++)
	{
		if(((tag)&(1<<i))==0)
			addr[14-i]='0';
		else	
			addr[14-i]='1';
	}

	for(i=0,j=15;i<=5;i++,j++)
	{
		if(((index)&(1<<i))==0)
			addr[20-i]='0';
		else	
			addr[20-i]='1';
	}
	//invalidate in L1

	//one of the address to invalidate
	addr[21]='0';
	delete_update(wb,addr,cache2);
	unsigned PADDR=convert_to_PADDR(addr);
	invalidate_in_L1(cache1,PADDR);

	//other address to invalidate
	addr[21]='1';
	delete_update(wb,addr,cache2);
	PADDR=convert_to_PADDR(addr);
	invalidate_in_L1(cache1,PADDR);

	L2_CACHE_ENTRY* rem1=L2_CACHE_lookup(cache2,PADDR);
	if(rem1->D!=0)
	{
		write_memory(X,(PADDR>>10),PID);
	}

}


//handle modified bit
void cache_controller(MEMORY *X, L1_CACHE *cache1,L2_CACHE *cache2,write_buffer *wb,unsigned int PADDR,int write, unsigned int PID)
{
	char addr[27];
	bin_string(addr,PADDR,27); //this addr has the 26bit physical address                 
	// printf("%s\n",addr);

	int pres_in_L1=0;
	int pres_in_L2=0;
	int pres_in_main=0;

	while(pres_in_L1==0)
	{
		
		L1_CACHE_ENTRY* presL1=L1_CACHE_lookup(cache1,PADDR);
		if(presL1!=NULL)
			pres_in_L1=1;

		if(pres_in_L1==1)
		{
			printf("L1 cache hit\n");
			cache1->hits++;
			if(write==1)
			{
				printf("Byte being written in the given address\n");
				add_write_buffer(wb,addr,cache2);
			}
			else
			{
				printf("Byte being read from the given address\n");
			}
		}
		else
		{
			printf("L1 cache miss\n");
			cache1->misses++;
			printf("Checking from L2 and main memory simultaneously\n");
			L2_CACHE_ENTRY* presL2=L2_CACHE_read(cache2,PADDR);
			if(presL2!=NULL)
				pres_in_L2=1;

			pres_in_main=1;

			if(pres_in_L2==1)
			{	
				printf("Data there in L2 and main memory\n");
				cache2->hits++;
				printf("Rejecting data of main memory\n");
				printf("There in L2, loading the block to L1\n");
				load_to_L1(cache1,cache2,wb,PADDR);
			}
			else if(pres_in_main==1)
			{
				printf("There in main memory, loading the block to L2\n");
				cache2->misses++;
				cache2->hits++;
				load_to_L2(X,cache1,cache2,wb,PADDR,PID);
				printf("There in L2 now, loading the block to L1\n");
				load_to_L1(cache1,cache2,wb,PADDR);
			}

			// else
			// {
			// 	//wont enter here ever 
			// 	printf("Loading the block to main memory");
			// 	load_to_main();
			// 	printf("There in main memory, loading the block to L2\n");
			// 	load_to_L2();
			// 	printf("There in L2 now, loading the block to L1\n");
			// 	load_to_L1();
			// }
		}
	}
}
/*
	these load to functions will also have to implement the replacement policies


// */
// int check_L1()
// {
// 	int flag=0; // flag to check if there in L1
// 	if(write==1)
// 	{
// 		check_in_L1_data(&flag);
// 	}
// 	else
// 	{
// 		check_in_L1_instr(&flag);
// 	}
// 	return flag;
// }

// int main()
// {
// 	L1_CACHE *L1 = (L1_CACHE*)malloc(sizeof(L1_CACHE));

// 	// cache on boot-up
// 	L1_CACHE_init(L1);

// 	L2_CACHE *L2 = (L2_CACHE*)malloc(sizeof(L2_CACHE));
// 	L2_CACHE_init(L2);

// 	write_buffer *wb=(write_buffer*)malloc(sizeof(write_buffer));
// 	initialize_wb(wb);

// 	unsigned int PADDR;
// 	L1_CACHE_ENTRY* cache_response;
// 	int write=0;

// 	// <5 ,105, 1> = 101 1101001 0001
// 	PADDR = 0b10111010010001;
// 	write=1;	
// 	cache_controller(L1,L2,wb,PADDR,write);
		



// 	// <4 ,105, 5> = 100 1101001 0101
// 	PADDR = 0b10011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <6 ,105, 5> = 110 1101001 0101
// 	PADDR = 0b11011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <7 ,105, 5> = 111 1101001 0101

// 	PADDR = 0b11111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);


// 	// <2 ,105, 5> = 010 1101001 0101
// 	PADDR = 0b01011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

	
// 	// < 18,105, 5> = 10010 1101001 0101
// 	PADDR = 0b1001011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);


// 	// <19 ,105, 5> = 10011 1101001 0101
// 	PADDR = 0b1001111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <20 ,105, 5> = 10100 1101001 0101
// 	PADDR = 0b1010011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <21 ,105, 5> = 10101 1101001 0101
// 	PADDR = 0b1010111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 		// <22 ,105, 5> = 10110 1101001 0101
// 	PADDR = 0b1011011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);


// 	// <23 ,105, 5> = 10111 1101001 0101
// 	PADDR = 0b1011111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <24 ,105, 5> = 11000 1101001 0101
// 	PADDR = 0b1100011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <25 ,105, 5> = 11001 1101001 0101
// 	PADDR = 0b1100111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);


// 		// <26 ,105, 5> = 11010 1101001 0101
// 	PADDR = 0b1101011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);


// 	// <27 ,105, 5> = 11011 1101001 0101
// 	PADDR = 0b1101111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <28 ,105, 5> = 11100 1101001 0101
// 	PADDR = 0b1110011010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// 	// <29 ,105, 5> = 11101 1101001 0101
// 	PADDR = 0b1110111010010101;
// 	write=1;
// 	cache_controller(L1,L2,wb,PADDR,write);

// }