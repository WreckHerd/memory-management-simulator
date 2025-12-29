#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <deque>

struct Block {
    int id;
    size_t size;
    size_t startAddress;
    bool isFree;
    Block* next;

    Block(int _id, size_t _sz, size_t _addr, bool _free, Block* _next = nullptr)
        : id(_id), size(_sz), startAddress(_addr), isFree(_free), next(_next) {}
};

struct cacheline
{
    size_t tag;
    bool valid;

    cacheline()
        :valid(false), tag(0){}

    cacheline(size_t _tag, bool _valid = false)
        :valid(_valid), tag(_tag){}
};

class cachelevel
{
private: 
    size_t lvl;
    size_t size;
    size_t blksize;
    std::vector <cacheline> lines;
    std::deque <int> fifoqueue{};


public:

//blksize fixed at 10, numlines = size/blksize
    cachelevel(size_t _lvl, size_t _size)
        :lvl(_lvl), size(_size), blksize(10), lines(std::vector<cacheline>(_size/10)) {}

    bool access(size_t addr)
    {
        size_t address = (addr/blksize)*blksize;
        if(lines[address % lines.size()].tag == address && lines[address % lines.size()].valid)
        {
            fifoqueue.push_back(address % lines.size());
            return true;
        }
        else
            return false;
    }

    void load(size_t addr)
    {
        size_t address = (addr/blksize)*blksize;

        lines[address % lines.size()].tag = address;
        lines[address % lines.size()].valid = true;

    }
};

class MemoryManager {
private:
    Block* head;
    int nextId{};
    size_t totalsize{};
    std::string allocstrat; 

    void mergeFreeBlocks(Block* blk1, Block* blk2) 
    {
        blk1->next = blk2->next;
        blk1->size = blk1->size + blk2->size;
        delete blk2;
    }

public:

    MemoryManager(size_t size, std::string strategy)
        : totalsize(size), nextId(1), head (new Block(-1, size, static_cast<size_t>(0), true, nullptr)), allocstrat(strategy) {}

    void malloc(size_t reqsize) 
    {
        if(allocstrat == "firstfit")
        {
            Block* current = head;
            bool done{false};
            while (current != nullptr)
            {
                if(current->isFree && current->size >= reqsize)
                {
                    done = true;
                    Block* newblk{};
                    if(current->size != reqsize)
                    {
                        newblk = new Block(-1, (current->size - reqsize), current->startAddress + reqsize, true, current->next);
                        current->next = newblk;
                        current->size = reqsize;
                    }
                    current->isFree = false;
                    current->id = nextId++;
                    break;
                }
                current = current->next;
            }
            if (!done)
            {
                //allocation failed
            }
        }

        else if(allocstrat == "bestfit")
        {
            Block* bestblk{};
            Block* current = head;

            while (current != nullptr)
            {
                if(current->isFree && current->size >= reqsize)
                {
                    if(!bestblk)
                    {
                        bestblk = current;
                    }
                    else if(bestblk->size > current->size)
                    {
                        bestblk = current;
                    }
                }
                current = current->next;
            }

            if(!bestblk)
            {
                //allocation failed
            }
            else if(bestblk->size == reqsize)
            {
                bestblk->isFree = false;
                bestblk->id = nextId++;
            }
            else if(bestblk->size > reqsize)
            {
                Block* newblk = new Block(-1, bestblk->size - reqsize, bestblk->startAddress + reqsize, true, bestblk->next);
                bestblk->next = newblk;
                bestblk->size = reqsize;
                bestblk->isFree = false;
                bestblk->id = nextId++;
            }
        }
        else if(allocstrat == "worstfit")
        {
            Block* worstblk{};
            Block* current = head;

            while (current != nullptr)
            {
                if(current->isFree && current->size >= reqsize)
                {
                    if(!worstblk)
                    {
                        worstblk = current;
                    }
                    else if(worstblk->size < current->size)
                    {
                        worstblk = current;
                    }
                }
                current = current->next;
            }

            if(!worstblk)
            {
                //allocation failed
            }
            else if(worstblk->size == reqsize)
            {
                worstblk->isFree = false;
                worstblk->id = nextId++;
            }
            else if(worstblk->size > reqsize)
            {
                Block* newblk = new Block(-1, worstblk->size - reqsize, worstblk->startAddress + reqsize, true, worstblk->next);
                worstblk->next = newblk;
                worstblk->size = reqsize;
                worstblk->isFree = false;
                worstblk->id = nextId++;
            }

        }
    }

    void free(int blockId)
    {
        Block* current = head;   

        //blockId should not be -1
        if(current->id == blockId)
        {
            current->isFree = true;
            current->id = -1;
            if(current->next->isFree)
            {
                mergeFreeBlocks(current, current->next);
            }
        }

        else
        {
            Block* prev = current;
            current = current->next;

            while(current != nullptr)
            {
                if(current->id == blockId)
                {
                    current->isFree = true;
                    current->id = -1;
                    if(current->next && current->next->isFree)
                    {
                        mergeFreeBlocks(current, current->next);
                    }
                    if(prev->isFree)
                    {
                        mergeFreeBlocks(prev, current);
                    }
                    break;
                }
                current = current->next;
                prev = prev->next;
            }
        }
    }

    void dump()
    {
        Block* current = head;
        while (current != nullptr)
        {
            std::cout << "start addr: " << current->startAddress <<  ", size: " << current->size << ", isFree: " << current->isFree << std::endl;;
            
            current = current->next;
        }
    }


    bool isValidAddress(size_t addr)
    {
        //add functionality to check if memory is free
        if (addr < totalsize)
            return true;
        else
            return false;
    }
};

class SystemSimulator
{
private:
    cachelevel l1;
    cachelevel l2;

public:
    MemoryManager mem;
    SystemSimulator(size_t memsize, std::string allocstrat, size_t cachesizel1, size_t cachesizel2) 
        : mem(MemoryManager(memsize, allocstrat)), l1(cachelevel(1, cachesizel1)), l2(cachelevel(1, cachesizel2)) {}

    void read(size_t address)
    {
        if(!mem.isValidAddress(address))
        {
            std::cout << "address is not valid" << std::endl;
            return; 
        }

        if(l1.access(address))
        {
            std::cout << "l1 cache hit" << std::endl;
        }
        else if (l2.access(address))
        {
            std::cout << "l2 cache hit" << std::endl;
            l1.load(address);
        }
        else
        {
            std::cout << "l1 and l2 miss" << std::endl;
            l1.load(address);
            l2.load(address);
        }

    }
};

int main()
{
    SystemSimulator proc(1024, "worstfit", 100, 200);
    proc.read(500);
    proc.read(1030);
    proc.read(500);



    proc.mem.dump();
    std::cout << "///" << std::endl;

    proc.mem.malloc(100);
    proc.mem.malloc(200);
    proc.mem.malloc(300);
    
    proc.mem.dump();
    std::cout << "///" << std::endl;

    proc.mem.free(2);

    proc.mem.dump();
    std::cout << "///" << std::endl;
    
    proc.mem.malloc(50);

    proc.mem.dump();

    std::cout << "///" << std::endl;
}        