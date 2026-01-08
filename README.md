## Install and Build

1.  Clone the repository:
    ```bash
    git clone [https://github.com/WreckHerd/memory-management-simulator.git](https://github.com/WreckHerd/memory-management-simulator.git)
    cd memory-management-simulator
    ```

2.  Compile the source code:
    ```bash
    g++ -I include src/main.cpp src/allocator/allocator.cpp src/cache/cache.cpp src/buddy/buddy.cpp -o memsim
    ```

## Usage
3. Execute

    ```bash
    ./memsim
    ```
    ### Required relations for a valid configuration:
    
    - 0 \< L1 size \< L2 size \< main memory size  
    - Buddy allocation requires memory size to be a power of two  
    - Line size must be a power of 2  
    - Cache size and main memory size must be multiple of line size  
    - Number of lines \= $\frac{L(1,2) size}{linesize}$, must be divisible by associativity  
    - Associativity \> 0
    
    The simulator won’t initialize until these requirements are fulfilled.  
    
    
    ### Initialize MemoryAllocator and Cache with desired conifgurations:
    - Do you want buddy allocation (N/y): n
    - Enter MainMemory size (bytes): 1024
    - Enter Allocation Strategy (firstfit / bestfit / worstfit): bestfit
    - Enter L1 Cache Size (bytes): 128
    - Enter L2 Cache Size (bytes): 512
    - Enter linesize (bytes): 16
    - Enter Cache associativity ((1)direct associative / (2)-way associative / (4)-way associative / (8)-way associative): 1
    - Enter Cache Replacement Policy (fifo / lru): lru

   ### Available commands
   - malloc <size>     : allaocate a block of <size> in memory
   - free <blockId>    : free the block with id <blockId
   - memdump           : print the current state of memeory
   - read <address>    : read data a <address> from cache or memory
   - write <address>   : write into <address>
   - stats             : print stats related to cache and memory
   - quit / exit       : exit the simulator

4. Alternatively

    if you want a quick run or want to test some feature you can use one of the trace files in tests/ as input
   
    ```bash
    ./memsim < tests/trace_allocator_bestfit.txt
    ```


## Demo Video

[![Watch the Simulator Demo](https://img.youtube.com/vi/kBQ6Q53O8VM/maxresdefault.jpg)](https://youtu.be/kBQ6Q53O8VM)

> Click the image above to watch the full walkthrough.
