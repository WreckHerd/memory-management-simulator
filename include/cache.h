#ifndef CACHE_LEVEL_H
#define CACHE_LEVEL_H

#include <vector>
#include <cstddef>
#include <string> 
#include <iostream>
//#include <deque>


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
    //std::deque <int> fifoqueue;
    //std::vector<cacheline> lines;
    //std::string org;

public:
    cachelevel(size_t _lvl, size_t _size, size_t assoc = 2, std::string repol = "lru", size_t linesize = 8);
    bool access(size_t addr);
    void load(size_t addr);
    writeresult write(size_t addr);
    void stats();
};

#endif