#ifndef CACHE_LEVEL_H
#define CACHE_LEVEL_H

#include <vector>
#include <cstddef>
#include <string> 
#include <deque>

struct cacheline {
    size_t tag;
    bool valid;
    cacheline(size_t _tag = 0, bool _valid = false);
};

class cachelevel {
private:
    size_t lvl;
    size_t size;
    size_t linesize;
    std::vector<cacheline> lines;
    std::string assoc;
    std::deque <int> fifoqueue;

public:
    cachelevel(size_t _lvl, size_t _size, std::string _assoc = "directmapped");
    bool access(size_t addr);
    void load(size_t addr);
};

#endif