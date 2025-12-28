#include <iostream>
#include <stdio.h>
#include <string.h>

struct Block {
    int id;
    size_t size;
    size_t startAddress;
    bool isFree;
    Block* next;

    Block(int _id, size_t _sz, size_t _addr, bool _free, Block* _next = nullptr)
        : id(_id), size(_sz), startAddress(_addr), isFree(_free), next(_next) {}
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


    // bool isValidAddress(size_t addr); 
};

int main()
{
    MemoryManager mem(1024, "worstfit");    
    mem.dump();
    std::cout << "///" << std::endl;

    mem.malloc(100);
    mem.malloc(200);
    mem.malloc(300);
    
    mem.dump();
    std::cout << "///" << std::endl;

    mem.free(2);

    mem.dump();
    std::cout << "///" << std::endl;
    
    mem.malloc(50);

    mem.dump();

    std::cout << "///" << std::endl;
}        