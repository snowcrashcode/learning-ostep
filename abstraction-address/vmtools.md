# In this homework, I'll learn about a few useful tools to examine virtual memory usage on  Linux-based systems. 

### 1) Read free's manual page. Free displays the amount of free and used physical and swap memory in system, as well as buffers and caches used by the kernel.

### total | used | free | shared | buffers | cache | buff/cache available | 

### 2) Run free -m (displays memory totals in megabytes). 7917 MB in system; 6616 MB is free. Does'nt match my intuition, considering my computer is supposed to have 16GB of RAM!

### 4) More memory is being used up. If too large, you get a segmentation fault.

### 5) pmap: Reports memory map of a process.

### 6) > ps auxw: Take 46611 for example. (/bin/bash)

### 7) pmap -X 46611 outputs many columns, including but not limited to: Referenced, Anonymous, LazyFree... This are, I presume, the memory map of that specific process. As for how many different entities? *Many.* There's code, heap, stack and shared libraries. 

### 8) Since program allocates a large amount of memory using calloc, this'll be reflected in the heap section of the memory map.