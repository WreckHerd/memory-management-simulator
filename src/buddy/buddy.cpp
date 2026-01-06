#include "buddy.h"

BuddyManager::BuddyManager(size_t memsize)
    :memsize(memsize)
{
    //initializing freelist vector with nullpointers 
    //where index of the vector contains a pointer to a linked list of free block of size 2^k

    for(int i{}; i <= std::log2(memsize); ++i)
    {
        if(i == std::log2(memsize))
        {
            freelist.emplace_back(new BNode(0, memsize, nullptr, true, -1));
        }
        freelist.emplace_back(nullptr);
    }
}

BNode* BuddyManager::trickle(int index)
{
    if(freelist[index])
    {
        //if free blk of size 2^index exists remove it from freelist and return it
        BNode* ret = freelist[index];
        freelist[index] = freelist[index]->next;
        return ret;
    }
    else
    {
        //else check if you are at the max index
        if(std::log2(memsize) == index)
        {
            std::cout << "Allocation failed: not enough memory\n";
            return nullptr;
        }
        else
        {
            //if not check for a free block of size 2^(index+1) or bigger
            BNode* ret = trickle(index+1);

            if(ret)
            {
                BNode* free_buddy = new BNode(ret->address + (ret->size/2), ret->size/2, nullptr, true, -1);
                freelist[index] = free_buddy;
                ret->size = ret->size/2;
                ret->next = nullptr;
                
                return ret;
            }
            else
                return nullptr;

        }
    }
}

void BuddyManager::malloc(size_t size)
{
    //k is the smallest int such that 2^k > size;
    int k = std::ceil(std::log2(size));

    if(freelist[k])
    {
        //free block of size 2^k exists

        BNode* allocatedblk = freelist[k];

        freelist[k] = freelist[k]->next;

        allocatedblk->next = nullptr;
        allocatedblk->id = ++nextid;
        allocatedblk->isFree = false;
        allocated_blocks[allocatedblk->id] = allocatedblk;
        std::cout << "allocated a block at address: " << allocatedblk->address << ", of size: " << allocatedblk->size << ", with blockId: " << allocatedblk->id << std::endl;
    }
    else
    {
        //else check if at max index
        if(std::log2(memsize) == k)
        {
            std::cout << "allocation failed: Not enought memory\n";
        }
        else
        {
            //if not check for a free block of size 2^(k+1) or bigger
            BNode* allocatedblk = trickle(k+1);

            if(allocatedblk)
            {
                BNode* free_buddy = new BNode(allocatedblk->address + (allocatedblk->size/2), allocatedblk->size/2, nullptr, true, -1);
                freelist[k] = free_buddy;
                allocatedblk->size = allocatedblk->size/2;
                allocatedblk->id = ++nextid;
                allocatedblk->isFree = false;
                allocatedblk->next = nullptr;
                allocated_blocks[allocatedblk->id] = allocatedblk;
                std::cout << "allocated a block at address: " << allocatedblk->address << ", of size: " << allocatedblk->size << ", with blockId: " << allocatedblk->id << std::endl;
            }
            else
            {
                return;
            }
        }
    }
}

void BuddyManager::free(int id)
{
    BNode* targetblk = allocated_blocks[id];
    int k = std::log2(targetblk->size);

    BNode* buddy{};
    size_t budaddress = targetblk->size ^ targetblk->address;
    BNode* current = freelist[k];

    BNode* prev{};

    //check if buddy is free
    while(current)
    {
        if(current->address == budaddress)
        {
            buddy = current;
            break;
        }
        if(!prev)
        {
            prev = freelist[k];
        }
        else
        {
            prev = prev->next;
        }
        current = current->next;
    }

    if(buddy)
    {
        //if buddy is free extend targetblk and delete buddy then recursively call free
        if(prev)
        {
            assert(prev->next == buddy && "prev should be behind buddy");
            prev->next = buddy->next;
        }
        else
        {
            freelist[k] = nullptr;
        }

        targetblk->size = 2*targetblk->size;
        targetblk->address = std::min(targetblk->address, buddy->address);
        delete buddy;

        free(targetblk->id);
    }
    else
    {
        targetblk->next = freelist[k];
        freelist[k] = targetblk;
        targetblk->id = -1;
        targetblk->isFree = true;
        allocated_blocks.erase(id);

        std::cout << "Freed block with id: " << id << ", of size:" << targetblk->size << std::endl;
    }

}