#include "allocator.h"

memNode::memNode(int _id, size_t _sz, size_t _addr, bool _free, memNode* _next)
    : id(_id), size(_sz), startAddress(_addr), isFree(_free), next(_next) {}


MemoryManager::MemoryManager(size_t size, std::string strategy)
    : totalsize(size), nextId(1), head (new memNode(-1, size, static_cast<size_t>(0), true, nullptr)), allocstrat(strategy) {}

MemoryManager::~MemoryManager()
{
    memNode* current = head;
    while(current)
    {
        memNode* nextnode = current->next;
        delete current;
        current = nextnode;
    }
}


void MemoryManager::mergeFreememNodes(memNode* blk1, memNode* blk2) 
{
    blk1->next = blk2->next;
    blk1->size = blk1->size + blk2->size;
    delete blk2;
}


void MemoryManager::malloc(size_t reqsize) 
{
    if(allocstrat == "firstfit")
    {
        memNode* current = head;
        bool done{false};
        while (current != nullptr)
        {
            if(current->isFree && current->size >= reqsize)
            {
                done = true;
                memNode* newblk{};
                if(current->size != reqsize)
                {
                    newblk = new memNode(-1, (current->size - reqsize), current->startAddress + reqsize, true, current->next);
                    current->next = newblk;
                    current->size = reqsize;
                }
                current->isFree = false;
                current->id = nextId;
                break;
            }
            current = current->next;
        }

        if (!done)
        {
            //allocation failed
            std::cout << "allocation failed" << std::endl;
            unsucallocs++;
            allocs++;
        }
        else
        {
            std::cout << "Allocated block id = " << nextId++ << " at address = " << current->startAddress << std::endl;
            allocs++;
        }

    }

    else if(allocstrat == "bestfit")
    {
        memNode* bestblk{};
        memNode* current = head;

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
            std::cout << "allocation failed" << std::endl;
            unsucallocs++;
            allocs++;
            return;
        }
        else if(bestblk->size == reqsize)
        {
            bestblk->isFree = false;
            bestblk->id = nextId++;
        }
        else if(bestblk->size > reqsize)
        {
            memNode* newblk = new memNode(-1, bestblk->size - reqsize, bestblk->startAddress + reqsize, true, bestblk->next);
            bestblk->next = newblk;
            bestblk->size = reqsize;
            bestblk->isFree = false;
            bestblk->id = nextId;
        }
        std::cout << "Allocated block id = " << nextId++ << " at address = " << bestblk->startAddress << std::endl;
        allocs++;

    }

    else if(allocstrat == "worstfit")
    {
        memNode* worstblk{};
        memNode* current = head;

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
            std::cout << "allocation failed" << std::endl;
            unsucallocs++;
            allocs++;
            return;
        }
        else if(worstblk->size == reqsize)
        {
            worstblk->isFree = false;
            worstblk->id = nextId++;
        }
        else if(worstblk->size > reqsize)
        {
            memNode* newblk = new memNode(-1, worstblk->size - reqsize, worstblk->startAddress + reqsize, true, worstblk->next);
            worstblk->next = newblk;
            worstblk->size = reqsize;
            worstblk->isFree = false;
            worstblk->id = nextId;
        }
        std::cout << "Allocated block id = " << nextId++ << " at address = " << worstblk->startAddress << std::endl;
        allocs++;

    }

}

void MemoryManager::free(int blockId)
{
    memNode* current = head;   
    bool done {false};
    bool merged {false};

    //blockId should not be -1
    if(current->id == blockId)
    {
        done = true;
        current->isFree = true;
        current->id = -1;
        std::cout << "Block with id " << blockId << " freed" << std::endl;

        if(current->next &&current->next &&  current->next->isFree)
        {
            mergeFreememNodes(current, current->next);
            std::cout << "free blocks merged" << std::endl;
        }

    }

    else
    {
        memNode* prev = current;
        current = current->next;

        while(current != nullptr)
        {
            if(current->id == blockId)
            {
                done = true;
                current->isFree = true;
                current->id = -1;
                std::cout << "Block with id " << blockId << " freed" << std::endl;

                if(current->next && current->next->isFree)
                {
                    merged = true;
                    mergeFreememNodes(current, current->next);
                }
                if(prev->isFree)
                {
                    merged = true;
                    mergeFreememNodes(prev, current);
                }
                if (merged)
                {
                    std::cout << "free blocks merged" << std::endl;
                }
                break;
            }
            current = current->next;
            prev = prev->next;
        }
    }

    if(!done)
    {
        //free failed
        std::cout << "No allocated block with the given id exists";
    }
}

void MemoryManager::dump()
{
    memNode* current = head;
    while (current != nullptr)
    {
        std::cout << "start addr: " << current->startAddress <<  ", size: " << current->size << ", isFree: " << current->isFree;
        if(!current->isFree)
            std::cout << ", block id: " << current->id << std::endl;
        else
            std::cout << std::endl;
        
        current = current->next;
    }
}

void MemoryManager::stats()
{
    memNode* current = head;
    int usedmem{};
    memNode* largestfreeblk{};

    std::cout << "Total memory: " << totalsize << std::endl;

    while(current != nullptr)
    {
        if(current->isFree)
        {
            if(!largestfreeblk)
            {
                largestfreeblk = current;
            }
            else
            {
                if(current->size > largestfreeblk->size)
                {
                    largestfreeblk = current;
                }
            }
        }
        else
            usedmem += current->size;

        current = current->next;
    }
    
    std::cout << "Used memory: " << usedmem << std::endl;
    if(totalsize != 0)
        std::cout << "Memory Utilization: " << (static_cast<double>(usedmem)/static_cast<double>(totalsize))*100.0 << "%" << std::endl;

    if(largestfreeblk && totalsize - usedmem  != 0)
        std::cout << "External Fregmentation: " << (1 - (static_cast<double>(largestfreeblk->size)/static_cast<double>(totalsize-usedmem)))*100.0 << "%" << std::endl;
        std:: cout << largestfreeblk->size << "  " << totalsize - usedmem << std::endl;
    if(allocs != 0)
        std::cout << "Allocation faliure rate: " << (static_cast<double>(unsucallocs)/static_cast<double>(allocs))*100.0 << "%" << std::endl;

}


bool MemoryManager::isValidAddress(size_t addr)
{
    if(addr > totalsize)
        return false;
    memNode* current = head;
    while(current && current->startAddress + current->size <= addr)
    {
        current = current->next;
    }
    if (current && !current->isFree)
        return true;
    else
        return false;
}