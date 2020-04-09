# Address-Translation-in-CPU

The memory subsystem [with TLB, L1 Cache, L2 Cache and Main Memory] has following configuration:

TLB: Identifier based TLB [with PID stored in each entry of the TLB. Invalidation to corresponding entries happens when a process terminates]. Number of entries in TLB: 32

L1 Cache: 8KB, 16B, 4 Way set associative cache. The cache follows Write buffer and Look through. It follows FIFO as replacement policy.

L2 Cache: 32KB, 32B, 16 Way set associative cache. The cache follows Write back and look aside. It follows LRU counter as replacement policy.

Main Memory with Memory Management: 64MB Main memory with Second chance as replacement policy. The memory management scheme used is Segmentation + Paging.


Other Assumptions: 

1. Virtual Memory contains the complete program. Hard disk is big enough to hold all the programs.
2. First 2 blocks of the process (assume the page size and frame size same and is 1KB) will be pre-paged into main memory before a process starts its execution.
3. All other pages are loaded on demand [Assume the system supports dynamic loading. Also assume that the system has no dynamic linking support].
4. Main memory follows Global replacement. Lower limit number of pages and upper limit number of pages per process should be strictly maintained.
5. Page tables of all the processes reside in main memory and will not be loaded into cache memory levels.


Execution Snapshots:

![Make Command](https://github.com/richidubey/Address-Translation-in-CPU/blob/master/screenshots/1.png?raw=true)

![In Between](https://github.com/richidubey/Address-Translation-in-CPU/blob/master/screenshots/2.png?raw=true)

![End of execution](https://github.com/richidubey/Address-Translation-in-CPU/blob/master/screenshots/3.png?raw=true)
