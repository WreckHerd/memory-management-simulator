#ifndef CACHE_LEVEL_H
#define CACHE_LEVEL_H

#include <vector>
#include <cstddef>
#include <string> 
//#include <deque>


struct cacheline {
    size_t tag;
    bool valid;
    cacheline(size_t _tag = 0, bool _valid = false);
};

class cacheset {
public:
    std::vector<cacheline> set;
    size_t capacity;

    cacheset(size_t capacity);
};

class cachelevel {
private:
    size_t lvl;
    size_t size;
    size_t linesize;
    std::vector<cacheline> lines;
    std::string org;
    size_t assoc;
    std::vector<cacheset> sets;
    bool fifo;
//   std::deque <int> fifoqueue;

public:
    cachelevel(size_t _lvl, size_t _size, std::string _assoc = "setassoc", size_t assoc = 2, bool fifo = true);
    bool access(size_t addr);
    void load(size_t addr);
};

#endif