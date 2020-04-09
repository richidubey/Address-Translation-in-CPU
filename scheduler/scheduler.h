#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESSES 5
#define NUM_PAGES_PREPAGED 2
#define BATCH_SIZE 64

#define PROCESS_DIR "./processes/"

typedef struct INFO
{
	unsigned int running;
} INFO;

typedef struct PCB
{
	FILE *fp;
	unsigned int pid: 3;
	unsigned int stbr: 26;
	unsigned int stlr: 26;
	unsigned int ptbr: 26;
	unsigned int ptlr: 26;
	unsigned int state: 1;
	unsigned int num_pages_prepaged;
	unsigned int batch_size;
} PCB;

void initialize_PCB(PCB *X, INFO *info, unsigned int PID, char *process_name);
unsigned int get_instruction(PCB *X, INFO *info);

#endif
