# all: tlb memory segtable pagetable fifo L1_CACHE L2_CACHE write_buffer cache_controller scheduler run clean
all: tlb.o memory.o segtable.o pagetable.o fifo.o L1_CACHE.o L2_CACHE.o write_buffer.o cache_controller.o execute run clean

tlb.o: tlb/tlb.c tlb/tlb.h
	@echo "creating object file for tlb"
	@gcc -c tlb/tlb.c

memory.o: memory/memory.c memory/memory.h
	@echo "creating object file for memory"
	@gcc -c memory/memory.c

segtable.o: memory/segtable.c memory/segtable.h memory/memory.h
	@echo "creating object file for segtable"
	@gcc -c memory/segtable.c	

pagetable.o: memory/pagetable.c memory/pagetable.h
	@echo "creating object file for pagetable"
	@gcc -c memory/pagetable.c

fifo.o: cache/FIFO/fifo.c cache/FIFO/fifo.h
	@echo "creating object file for fifo"
	@gcc -c cache/FIFO/fifo.c	

L1_CACHE.o: cache/L1_CACHE.c cache/L1_CACHE.h
	@echo "creating object file for L1_CACHE"
	@gcc -c cache/L1_CACHE.c

L2_CACHE.o: cache/L2_CACHE.c cache/L2_CACHE.h
	@echo "creating object file for L2_CACHE"
	@gcc -c cache/L2_CACHE.c

write_buffer.o: cache/write_buffer.c cache/write_buffer.h
	@echo "creating object file for write_buffer"
	@gcc -c cache/write_buffer.c

cache_controller.o: cache/cache_controller.c cache/cache_controller.h
	@echo "creating object file for cache_controller"
	@gcc -c cache/cache_controller.c

execute: scheduler/scheduler.h tlb.o memory.o segtable.o pagetable.o fifo.o L1_CACHE.o L2_CACHE.o write_buffer.o cache_controller.o
	@echo "creating object file for scheduler"
	@gcc -o execute scheduler/scheduler.c tlb.o memory.o segtable.o pagetable.o fifo.o L1_CACHE.o L2_CACHE.o write_buffer.o cache_controller.o

run:
	@echo "running program"
	@./execute

clean: 
	@echo "cleaning up"
	@rm *.o
	@rm execute
