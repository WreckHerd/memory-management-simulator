#include "cache.h"


cacheline::cacheline(size_t _tag, bool _valid)
    :valid(_valid), tag(_tag){}


//linesize fixed at 10, numlines = size/linesize
cachelevel::cachelevel(size_t _lvl, size_t _size, std::string _assoc)
    :lvl(_lvl), size(_size), linesize(8), lines(std::vector<cacheline>(_size/8)), assoc(_assoc) {}

bool cachelevel::access(size_t addr)
{
    size_t blkaddress = (addr/linesize);

    if(assoc == "directmapped")
    {
        size_t index = blkaddress % lines.size();
        if(lines[index].tag == blkaddress && lines[index].valid)
            return true;
        else
            return false;
    }
    else if(assoc == "setassoc")
    {
        size_t set = blkaddress % (lines.size()/2);
        if(lines[set*2].tag == blkaddress)
        {
            return true;
        }
    }

    return false;
}

void cachelevel::load(size_t addr)
{
    size_t blkaddress = (addr/linesize);
    size_t index = blkaddress % lines.size();

    lines[index].tag = blkaddress;
    lines[index].valid = true;

}

