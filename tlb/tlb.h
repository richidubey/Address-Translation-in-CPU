#ifndef TLB_H
#define TLB_H

#define TLB_SIZE 32

typedef struct TLB_ENTRY
{
	unsigned int pid: 3;
	unsigned int page_number: 22;
	unsigned int frame_number: 16;
	unsigned int valid: 1;
	unsigned int used: 1;
	unsigned int counter: 5;
} TLB_ENTRY;

typedef struct TLB
{
	TLB_ENTRY *entry;
	unsigned int TLB_size;
	unsigned int entries_used;
	unsigned int hits;
	unsigned int misses;
} TLB;

unsigned int min(int x, int y);
unsigned int max(int x, int y);

void bin(unsigned int N);
void bin_string(char *s, unsigned int, int digits);

void initialize_TLB(TLB *X, int N);
void show_TLB(TLB *X);

unsigned int get_TLB_slot(TLB *X);
void update_TLB_counters(TLB *X, unsigned int ENTRY_NUMBER, unsigned int HIT);
void update_TLB(TLB *X, unsigned int ENTRY_NUMBER, unsigned int PAGE_NUMBER, unsigned int FRAME_NUMBER, unsigned int PID, unsigned int HIT);
void invalidate_process_TLB(TLB *X, unsigned int PID);
unsigned int find(TLB *X, unsigned int LADDR, unsigned int PID, char TYPE);
unsigned int resolve(unsigned int LADDR);

#endif