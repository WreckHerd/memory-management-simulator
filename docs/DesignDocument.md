# Design Document \- Memory management simulator

# Allocator:

The Physical Memory simulator uses Linked List to represent blocks of memory internally.  
Nodes of the linked list are described by a struct with the following signature:
```
struct memNode {  
    int id;  
    size_t size;  
    size_t startAddress;  
    bool isFree;  
    memNode* next;

    memNode(int _id, size_t _sz, size_t _addr, bool _free, memNode* _next = nullptr);  
};
```
The class MemoryManager handles all functionality relating to allocation, deallocation and accounting of memory. void malloc(size\_t reqsize) can be used to allocate a memory block of a certain size (in bytes) with allocation strategy (firstfit / bestfit / worstfit) as specified by the user.

The class signature is:
```
class MemoryManager {  
private:  
    memNode* head;  
    int nextId;  
    size_t totalsize;  
    std::string allocstrat;  
    int allocs{};  
    int unsucallocs{};

   void mergeFreememNodes(memNode* blk1, memNode* blk2);

public:  
    MemoryManager(size_t size, std::string strategy);  
    ~MemoryManager();

   void malloc(size_t reqsize);  
    void free(int blockId);  
    void dump();  
    void stats();  
    bool isValidAddress(size_t addr);  
};
```

This class also provides functions to free a block of memory by specifying its blockId and a function void dump() to visualise the current state of memory.

The allocation strategy's implementation is as simple as linked list traversal to find the first, best or worst(largest) free block that can house the block size malloced.

**Stats of MemoryManager:**

Internal fragmentation while not using the buddy allocator is practically 0 (ignoring meta data) as memory is allocated by “splitting blocks if necessary” as specified in the document provided. Which results in an allocated block always exactly fitting the requested size.

External fragmentation is calculated using the formula:  

(1 \- $\frac{size of the largest free block}{Total free memory}$ ) * 100

Allocation failure rate and memory utilisation is also reported.

# Cache:

Cache hierarchy in the simulator follows 

1. Cache level L1  
2. Cache level L2  
3. Main memory 

For reads, caches are inclusive ie. L2 will contain a copy of L1 at all times \+ extra data.   
While write policy follows Write-Back, on write miss block is allocated only to L1 (not L2) and a dirty bit is flipped to signify a modified block.

Dirty blocks are evicted and subsequently written into lower levels on replacement.

Cache is implemented using the following structs and classes
```
struct cacheline {  
    size_t tag;  
    bool valid;  
    bool dirty;  
    cacheline(size_t _tag = 0, bool _valid = false, bool _dirty = false);  
};

class cacheset {  
public:  
    std::vector<cacheline> set;  
    size_t capacity;

    cacheset(size_t capacity);  
};

struct writeresult {  
    bool wasHit;  
    bool wasEvicted;  
    size_t address;

   writeresult(bool _wasEvicted, size_t _addr, bool _wasHit)  
        :wasEvicted(_wasEvicted), address(_addr), wasHit(_wasHit) {}  
};

class cachelevel {  
private:  
    size_t lvl;  
    size_t size;  
    size_t linesize;  
    size_t assoc;  
    std::vector<cacheset> sets;  
    std::string repol;  
    size_t hits{};  
    size_t misses{};

public:  
    cachelevel(size_t _lvl, size_t _size, size_t assoc = 2, std::string repol = "lru", size_t linesize = 8);  
    bool access(size_t addr);  
    void load(size_t addr);  
    writeresult write(size_t addr);  
    void stats();  
};
```
![20260105_145011](https://github.com/user-attachments/assets/76a01007-3af6-4b3f-bd7d-283acfb0e3dc)


The above diagram represents the relation between the 3 class signatures.

Almost all variables are configurable at the beginning of the simulation but they must follow certain relations to make sure the simulator behaves consistently.

Cache associativity can be set to values like (1) for direct associative, (2 / 4 / 8\) for 2-way, 4-way or 8-way associative cache.

For associativity \> 1 replacement policies implemented are 

- Fifo  
- Lru

Both of them are implemented using std::vector.

Linked list \+ hash table implementation of lru results in O(1) time complexity for access and load functions as compared to O(n) for std::vector where n is the associativity for this simulation.  
Direct and 2-way associative are the most commonly seen and anything above 4 or 8 being rare and elements of std::vector are contiguously allocated as compared to linked list in which accessing the next element is likely to result in a cache miss  
Because of the above reasons, std::vector based implementation was chosen for the simulation.

**Cache Stats:**

Cache hits and misses per level are reported.

Cache hit ratio is reported.

Average memory access time is calculated and reported by assuming that   
Accessing L1 takes 1 cycle  
Accessing L2 takes 10 cycles and  
Accessing the main memory takes 100 cycles.  
Reference formula for AMAT \= HitTimeL1 \+ (MissRateL1\*(HitTimeL2 \+ MissRateL2\* PenaltyMainMemory))

# Buddy Allocator:

Buddy allocator uses std::vector freelist to keep track of free blocks, where every index k contains a pointer to a linked list containing all free memory blocks with size 2^k.   
Std::unordered\_map allocated\_blocks, is a hashmap between block id and pointer to the node with that id. 

Struct BNode describes the list node for but containers. Its signature is:
```
struct BNode {  
public:  
    size_t address;  
    size_t size;  
    BNode* next{};  
    bool isFree;  
    int id;

    BNode(size_t _address, size_t _size, BNode* _next, bool _isFree, int _id)  
        :address(_address), size(_size), next(_next), isFree(_isFree), id(_id) {}  
};
```
BuddyManager class brings it all together and defines member functions void malloc(size\_t size), void free(size\_t size) used to allocate and free memory using the buddy scheme and a function BNode\* trickle(int index) which is a helper function to malloc called during recursive calls. Free handles recursive calls within itself.

Class signature of BuddyManger is:
```  
class BuddyManager {  
private:  
    size_t memsize;  
    std::vector<BNode*> freelist;  
    std::unordered_map<int, BNode*> allocated_blocks;  
    int nextid{};

public:  
    BuddyManager(size_t memsize);  
    ~BuddyManager();  
    void malloc(size_t address);  
    void free(int Id);  
    BNode* trickle(int index);  
};
```
Stats of buddy allocation are not implemented as they were not explicitly specified as required in the doc provided.

# Required relations for a valid configuration:

- 0 \< L1 size \< L2 size \< main memory size  
- Buddy allocation requires memory size to be a power of two  
- Line size must be a power of 2  
- Cache size and main memory size must be multiple of line size  
- Number of lines \= $\frac{L(1,2) size}{linesize}$, must be divisible by associativity  
- Associativity \> 0

The simulator won’t initialize until these requirements are fulfilled.  


