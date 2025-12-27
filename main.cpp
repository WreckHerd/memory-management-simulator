#include <iostream>
#include <bits/stdc++.h>

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
    std::string allstrategy; 

    void mergeFreeBlocks(Block* blk1, Block* blk2) 
    {
        blk1->next = blk2->next;
        blk1->size = blk1->size + blk2->size;
    }

public:

    MemoryManager(size_t size, std::string strategy)
        : totalsize(size), nextId(1), head (new Block(-1, size, static_cast<size_t>(0), true, nullptr)), allstrategy(strategy) {}

    void malloc(size_t reqsize) 
    {
        if(allstrategy == "firstfit")
        {
            Block* current = head;
            while (current != nullptr)
            {
                if(current->isFree && current->size >= reqsize)
                {
                    Block* newblk{};
                    if(current->size != reqsize)
                    {
                        newblk = new Block(-1, (current->size - reqsize), current->startAddress + reqsize, true, current->next);
                    }
                    current->next = newblk;
                    current->size = reqsize;
                    current->isFree = false;
                    current->id = nextId++;
                    break;
                }
                current = current->next;
            }
        }
    }

    void free(int blockId)
    {
        Block* current = head;   
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
                    if(current->next->isFree)
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
    MemoryManager mem(1024, "firstfit");    
    mem.dump();
    std::cout << "///" << std::endl;

    mem.malloc(100);
    mem.malloc(200);
    
    mem.dump();
    std::cout << "///" << std::endl;

    mem.free(1);

    mem.dump();
    std::cout << "///" << std::endl;
}        
    
    
    