#include "L1_CACHE.h"
#include "L2_CACHE.h"
#define SIZE_OF_BUFFER 8

#ifndef WRITE_BUFFER_H
#define WRITE_BUFFER_H

typedef struct write_buffer
{
	char address_buffer[SIZE_OF_BUFFER][27];
	int number_of_elements;
	//8 is size of write buffer
}write_buffer;


void bin_string(char *s, unsigned int N, int digits);
//functions needed

//search in write buffer and returns index from 0 to size-1 where it is
int search_write_buffer(write_buffer *wb,char *add, int index_of_bits);


//add entry to write buffer
void add_write_buffer(write_buffer *wb,char *add,L2_CACHE *cache2);
//utility function for the above task
void add_one_entry_write_buffer(write_buffer *wb,char *add);


//delete entry from write buffer
//given an index clears that position
//also decrements count
void delete_write_buffer(write_buffer *wb,int index);

void initialize_wb(write_buffer *wb);

//update from write_buffer_to_L2
//make this for putting change of one entry to L2
//while doing ^ decrement number_of_elements


unsigned convert_to_PADDR(char *addr);
void update_buffer_to_L2(int wb_index,write_buffer *wb,L2_CACHE *cache2);	


#endif



//choices 
//in write buffer when full, only send one entry to L2