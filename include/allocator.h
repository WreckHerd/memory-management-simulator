#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <string>
#include <cstddef>

struct memNode {
    int id;
    size_t size;
    size_t startAddress;
    bool isFree;
    memNode* next;

    memNode(int _id, size_t _sz, size_t _addr, bool _free, memNode* _next = nullptr);
};

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

#endif