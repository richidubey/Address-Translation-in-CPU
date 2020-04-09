#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scheduler.h"
#include "../tlb/tlb.h"
#include "../memory/memory.h"
#include "../memory/segtable.h"
#include "../memory/pagetable.h"
#include "../cache/L1_CACHE.h"
#include "../cache/L2_CACHE.h"
#include "../cache/write_buffer.h"
#include "../cache/cache_controller.h"

void initialize_process(PCB *X, INFO *info, unsigned int PID, char *process_name)
{
	char filename[256];

	strcpy(filename, PROCESS_DIR);
	strcat(filename, process_name);

	FILE *fp = fopen(filename, "r");

	X->fp = fp;
	X->pid = PID;
	X->state = 0;
	X->num_pages_prepaged = NUM_PAGES_PREPAGED;
	X->batch_size = BATCH_SIZE;
	info->running = info->running+1;
}

unsigned int get_instruction(PCB *X, INFO *info)
{
	unsigned int LAddr = 0;
	if(fscanf(X->fp, "%x", &LAddr) == -1)
		info->running = info->running-1;
	return LAddr;
}

int main()
{
	char *process_name[5] = {"APSI.txt", "CC1.txt", "LI.txt", "M88KSIM.txt", "VORTEX.txt"};

	int num_processes;
	printf("Enter the number of processes: ");
	scanf("%d", &num_processes);

	if(num_processes <= 0)
	{
		printf("Min Processes allowed: %d\n", 1);
		return 0;
	}	

	if(num_processes > MAX_PROCESSES)
	{
		printf("Max Processes allowed: %d\n", MAX_PROCESSES);
		return 0;
	}

	//initialization
	
	INFO *info = (INFO *)malloc(sizeof(INFO));

	TLB *tlb = (TLB *)malloc(sizeof(TLB));

	MEMORY *memory = (MEMORY *)malloc(sizeof(MEMORY));

	SEGTABLE **local_segtable = (SEGTABLE **)malloc(num_processes*sizeof(SEGTABLE *));
	SEGTABLE **global_segtable = (SEGTABLE **)malloc(num_processes*sizeof(SEGTABLE *));

	PAGETABLE **pagetable = (PAGETABLE **)malloc(num_processes*sizeof(PAGETABLE *));

	PCB **pcb = (PCB **)malloc(num_processes*sizeof(PCB *));

	L1_CACHE *L1 = (L1_CACHE*)malloc(sizeof(L1_CACHE));

	L2_CACHE *L2 = (L2_CACHE*)malloc(sizeof(L2_CACHE));

	write_buffer *wb = (write_buffer*)malloc(sizeof(write_buffer));

	for(int i = 0; i<num_processes; i++)
	{
		local_segtable[i] = (SEGTABLE *)malloc(sizeof(SEGTABLE));
		global_segtable[i] = (SEGTABLE *)malloc(sizeof(SEGTABLE));
	}

	for(int i = 0; i<num_processes; i++)
		pagetable[i] = (PAGETABLE *)malloc(num_processes*sizeof(PAGETABLE));

	for(int i = 0; i<num_processes; i++)
		pcb[i] = (PCB *)malloc(sizeof(PCB));

	initialize_TLB(tlb, TLB_SIZE);

	initialize_memory(memory, MEMORY_SIZE);

	for(int i = 0; i<num_processes; i++)
	{
		initialize_segtable(local_segtable[i], SEGTABLE_SIZE);
		initialize_segtable(global_segtable[i], SEGTABLE_SIZE);
	}

	for(int i = 0; i<num_processes; i++)
		initialize_pagetable(pagetable[i], NUM_PAGETABLES, PAGETABLE_SIZE);

	L1_CACHE_init(L1);
	L2_CACHE_init(L2);

	initialize_wb(wb);

	for(int i = 0; i<num_processes; i++)
		initialize_process(pcb[i], info, i, process_name[i]);

	//pre-paging

	for(int i = 0; i<num_processes; i++)
	{
		printf("Pre-paging for PID: %d\n\n", pcb[i]->pid);
		for(int j = 0; j<pcb[i]->num_pages_prepaged; j++)
		{
			unsigned int LAddr = get_instruction(pcb[i], info);
			unsigned int Offset = (LAddr&(1<<10-1));
			unsigned int ptbr, frame;
			unsigned int pid = pcb[i]->pid;
		
			ptbr = get_page_table_address(memory, L1, L2, wb, local_segtable[pid], pid, LAddr>>29);
			frame = get_frame_address(memory, L1, L2, wb, pagetable[pid], pid, ptbr, ((LAddr & 0x1ffffc00 )>>10));

			// printf("TLB hits: %u, TLB misses: %u\nTLB hit ratio: %f\n\n", tlb->hits, tlb->misses, (float)(tlb->hits)/(tlb->hits+tlb->misses));
			// printf("L1 cache hits: %d, L1 cache misses: %d\nL1 cache hit ratio: %f\n\n", L1->hits, L1->misses, (float)(L1->hits)/(L1->hits+L1->misses));
			// printf("L2 cache hits: %d, L2 cache misses: %d\nL2 cache hit ratio: %f\n\n", L2->hits, L2->misses, (float)(L2->hits)/(L2->hits+L2->misses));

			// show_TLB(tlb);
			// L1_CACHE_show(L1, 0, L1->n_rows);
			// L2_CACHE_show(L2, 0, L2->n_rows);
			printf("\n--------------------------------------------------------------------------------\n\n");
			// }
		}
		pcb[i]->state = 1;
	}


	printf("\n********************************************************************************\n");
	printf("\nAll processes prepaged in memory\n");
	printf("\n********************************************************************************\n\n");

	//resetting file pointers

	for(int i = 0; i<num_processes; i++)
		rewind(pcb[i]->fp);

	//run processes

	while(info->running)
	{
		for(int i = 0; i<num_processes; i++)
		{		
			for(int j = 0; j<pcb[i]->batch_size; j++)
			{
				if(pcb[i]->state == 1)
				{
					unsigned int LAddr = get_instruction(pcb[i], info);
					unsigned int Offset = (LAddr&(1<<10-1));
					unsigned int ptbr, frame;
					unsigned int pid = pcb[i]->pid;
					unsigned int write = ((LAddr>>28) == 7)?1:0;
					printf("PID: %u, LADDRESS: %x\n", pid, LAddr);
					if(LAddr == 0)
					{
						invalidate_process_TLB(tlb, pid);
						process_dies(memory,L1,L2,wb,pid);					
						pcb[i]->state = 0;
					}
					else
					{
						if(find(tlb, LAddr, pid, 'X'))
						{
							frame = find(tlb, LAddr, pid, 'F');
						}
						else
						{
							ptbr = get_page_table_address(memory, L1, L2, wb, local_segtable[pid], pid, LAddr>>29);
							frame = get_frame_address(memory, L1, L2, wb, pagetable[pid], pid, ptbr, ((LAddr & 0x1ffffc00 )>>10));
							int ENTRY_NUMBER = get_TLB_slot(tlb);
							update_TLB(tlb, ENTRY_NUMBER, LAddr>>10, frame, pid, 0);
							find(tlb, LAddr, pid, 'X');
						}
						unsigned int PADDR = (frame<<10) + Offset;
						printf("PADDRESS: %x\n", PADDR);
						cache_controller(memory,L1,L2,wb,PADDR,write,pid);

						// printf("TLB hits: %u, TLB misses: %u\nTLB hit ratio: %f\n\n", tlb->hits, tlb->misses, (float)(tlb->hits)/(tlb->hits+tlb->misses));
						// printf("L1 cache hits: %d, L1 cache misses: %d\nL1 cache hit ratio: %f\n\n", L1->hits, L1->misses, (float)(L1->hits)/(L1->hits+L1->misses));
						// printf("L2 cache hits: %d, L2 cache misses: %d\nL2 cache hit ratio: %f\n\n", L2->hits, L2->misses, (float)(L2->hits)/(L2->hits+L2->misses));

						// show_TLB(tlb);
						// L1_CACHE_show(L1, 0, L1->n_rows);
						// L2_CACHE_show(L2, 0, L2->n_rows);
						printf("\n--------------------------------------------------------------------------------\n\n");
					}
				}
			}
		}
	}

	printf("\n********************************************************************************\n");
	printf("\nAll processes ran successfully\n");
	printf("\n********************************************************************************\n\n");
	
	printf("Statistics:\n\n");
	
	printf("TLB hits: %u, TLB misses: %u\nTLB hit ratio: %f\n\n", tlb->hits, tlb->misses, (float)(tlb->hits)/(tlb->hits+tlb->misses));
	printf("L1 cache hits: %d, L1 cache misses: %d\nL1 cache hit ratio: %f\n\n", L1->hits, L1->misses, (float)(L1->hits)/(L1->hits+L1->misses));
	printf("L2 cache hits: %d, L2 cache misses: %d\nL2 cache hit ratio: %f\n\n", L2->hits, L2->misses, (float)(L2->hits)/(L2->hits+L2->misses));

	return 0;
}
