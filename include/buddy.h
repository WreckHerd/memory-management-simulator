#ifndef BUDDY_H
#define BUDDY_H

#include <vector>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <cassert>

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

class BuddyManager {
private:
    size_t memsize;
    std::vector<BNode*> freelist;
    std::unordered_map<int, BNode*> allocated_blocks;
    int nextid{};

public:
    BuddyManager(size_t memsize);
    void malloc(size_t address);
    void free(int Id);
    BNode* trickle(int index);

};


#endif