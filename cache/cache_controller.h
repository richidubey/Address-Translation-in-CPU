#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "write_buffer.h"
#include "L1_CACHE.h"
#include "L2_CACHE.h"
#include "../memory/memory.h"

void delete_update(write_buffer *wb, char *addr,L2_CACHE *cache2);
void L1_replacer(L1_CACHE *cache1,L2_CACHE* cache2,L1_CACHE_ENTRY *entry,write_buffer * wb,unsigned index);
void L2_replacer(MEMORY*X,L1_CACHE *cache1,L2_CACHE *cache2,L2_CACHE_ENTRY *entry2,write_buffer* wb,unsigned index,unsigned int PID);
void load_to_L1(L1_CACHE *cache1,L2_CACHE* cache2,write_buffer *wb ,unsigned PADDR);
void load_to_L2(MEMORY *X,L1_CACHE *cache1,L2_CACHE* cache2,write_buffer *wb ,unsigned PADDR,unsigned PID);
void invalidate_in_L1(L1_CACHE *cache1,unsigned PADDR);
void cache_controller(MEMORY *X,L1_CACHE *cache1,L2_CACHE *cache2,write_buffer *wb,unsigned int PADDR,int write,unsigned int PID);
void invalidate_cache(MEMORY *X,L1_CACHE*cache1, L2_CACHE*cache2,write_buffer *wb, unsigned PADDR,unsigned int PID);
#endif