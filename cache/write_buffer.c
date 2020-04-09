#include "L1_CACHE.h"
#include "L2_CACHE.h"
#include "write_buffer.h"
#include <stdio.h>
#include <stdlib.h>

/* to be commented */


// void bin_string(char *s, unsigned int N, int digits)
// {
// 	for(int i = 1; i<digits; i++)
// 		s[digits-i-1] = (N&(1<<(i-1)))?'1':'0';
// 	s[digits-1] = '\0';
// }

/*till here*/

int search_write_buffer(write_buffer *wb,char *add, int index_of_bits)
{
	//index_of_bits defines till which index to compare 
	//all bits need not be compared

	int i,j;
	for(i=0;i<SIZE_OF_BUFFER;i++)
	{	
		//means empty
		for(j=0;j<=index_of_bits;j++)
		{
			if(add[j]!=wb->address_buffer[i][j])
				break;
		}
		if(j==index_of_bits+1)
			return i;	
	}
	return -1;
}

void add_write_buffer(write_buffer *wb,char *add,L2_CACHE *cache2)
{ 
	int index_add=search_write_buffer(wb,add,21);
	if(index_add!=-1)
	{
		printf("Deleting old entry of block from write-buffer\n");
		printf("Adding new entry of block to write-buffer\n");
		return;
	}

	if(wb->number_of_elements<SIZE_OF_BUFFER)
	{
		add_one_entry_write_buffer(wb,add);
	}
	else
	{
		printf("Write-buffer is full\n");
		printf("Clearing the write-buffer, updating all the blocks to L2\n");

		//for all entries in size
		//call update_buffer_to_L2();
		for(int x=0;x<SIZE_OF_BUFFER;x++)
		{
			update_buffer_to_L2(x,wb,cache2);
		}
		add_one_entry_write_buffer(wb,add);
	}
}

void add_one_entry_write_buffer(write_buffer *wb,char *add)
{
	printf("Adding new entry of block to write-buffer\n");
	int i,j;
	for(i=0;i<SIZE_OF_BUFFER;i++)
	{
		if(wb->address_buffer[i][0]=='\0')
		{
			//means empty
			for(j=0;j<=26;j++)
			{
				wb->address_buffer[i][j]=add[j];
			}
			break;
		}	
	}
	wb->number_of_elements++;
}

void delete_write_buffer(write_buffer *wb,int index)
{
	int j;
	for(j=0;j<=26;j++)
	{
		wb->address_buffer[index][j]='\0';
	}	
	wb->number_of_elements--;
}

unsigned convert_to_PADDR(char *addr)
{
	unsigned PADDR=0;
	int i;
	for(i=0;i<=25;i++)
	{
		PADDR+=(addr[25-i]-'0')*(1<<i);
	}
	return PADDR;
}

void update_buffer_to_L2(int wb_index,write_buffer *wb,L2_CACHE *cache2)
{
	unsigned PADDR=convert_to_PADDR(wb->address_buffer[wb_index]);
	// L2_CACHE_ENTRY* entry=L2_CACHE_lookup(cache2,PADDR);
	// entry->D=1; //making modified as 1
	L2_CACHE_ENTRY *entry=L2_CACHE_write(cache2,PADDR);
	delete_write_buffer(wb,wb_index);
}

void initialize_wb(write_buffer *wb)
{
	int i,j;
	for(i=0;i<SIZE_OF_BUFFER;i++)
	{	
		//means empty
		for(j=0;j<=26;j++)
		{
			wb->address_buffer[i][j]='\0';
		}	
	}
	wb->number_of_elements=0;
}
// int main()
// {
// 	write_buffer wb;	
// 	int i,j;
// 	initialize_wb(&wb);
// 	printf("%d\n",wb.number_of_elements);
// 	L2_CACHE *L2 = (L2_CACHE*)malloc(sizeof(L2_CACHE));
// 	L2_CACHE_init(L2);

// 	for(i=0;i<20;i++)
// 	{
// 		printf("Choose which query\n");
// 		int q;
// 		scanf("%d",&q);
// 		if(q==1)
// 		{
// 			printf("Give entry to delete\n");
// 			int index;
// 			scanf("%d",&index);
// 			delete_write_buffer(&wb,index);
// 			printf("%d\n",wb.number_of_elements);
// 		}
// 		else if(q==2)
// 		{
// 			unsigned int N;
// 			scanf("%u",&N);
// 			char addr[27];
// 			bin_string(addr,N,27); //this addr has the 26bit physical address                 
// 			printf("%s\n",addr);
// 			add_write_buffer(&wb,addr,L2);
// 			printf("%d\n",wb.number_of_elements);
// 		}
// 	}
// }