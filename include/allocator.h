#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <string>
#include <cstddef>

struct Block {
    int id;
    size_t size;
    size_t startAddress;
    bool isFree;
    Block* next;

    Block(int _id, size_t _sz, size_t _addr, bool _free, Block* _next = nullptr);
};

class MemoryManager {
private:
    Block* head;
    int nextId;
    size_t totalsize;
    std::string allocstrat;
    int reads;
    int unsucreads;

    void mergeFreeBlocks(Block* blk1, Block* blk2);

public:
    MemoryManager(size_t size, std::string strategy);

    void malloc(size_t reqsize);
    void free(int blockId);
    void dump();
    void stats();
    bool isValidAddress(size_t addr);
};

#endif