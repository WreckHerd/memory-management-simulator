#include "cache.h"


cacheline::cacheline(size_t _tag, bool _valid)
    :valid(_valid), tag(_tag){}

cacheset::cacheset(size_t _cap)
    :capacity(_cap){}


//linesize fixed at 8
cachelevel::cachelevel(size_t _lvl, size_t _size, std::string _org, size_t _assoc, bool _fifo)
    :lvl(_lvl), size(_size), linesize(8), lines(std::vector<cacheline>(_size/linesize)), org(_org), assoc(_assoc), fifo(_fifo) {
        
        if(fifo)
        {
            for (int i {}; i < lines.size(); ++i)
            {
                sets.emplace_back(assoc);
            }
        }
    }

bool cachelevel::access(size_t addr)
{
    size_t blkaddress = (addr/linesize);

    if(org == "directmapped")
    {
        size_t numsets = lines.size();

        size_t index = blkaddress % numsets;
        size_t tag = blkaddress / numsets;

        if(lines[index].tag == tag && lines[index].valid)
            return true;
        else
            return false;
    }
    else if(org == "setassoc")
    {
        size_t numsets = lines.size()/assoc;

        size_t index = blkaddress % (numsets);
        size_t tag = blkaddress / numsets;

        if(fifo)
        {

            for (auto it = sets[index].set.begin(); it != sets[index].set.end(); ++it)
            {
                if(it->valid && it->tag == tag)
                    return true;
            }

            return false;
        }

    }

    return false;
}

void cachelevel::load(size_t addr)
{
    size_t blkaddress = (addr/linesize);

    if(org == "directmapped")
    {
        size_t numsets = lines.size();
        size_t index = blkaddress % numsets;
        size_t tag = blkaddress / numsets;

        lines[index].tag = tag;
        lines[index].valid = true;
    }

    else if(org == "setassoc")
    {
        size_t numsets = lines.size()/assoc;
        size_t index = blkaddress % numsets;
        size_t tag = blkaddress / numsets;

        if(fifo)
        {
            for (auto it = sets[index].set.begin(); it != sets[index].set.end();++it)
            {
                if(it->valid; it->tag == tag)
                    return;
            }

            if (sets[index].set.size() == sets[index].capacity)
                sets[index].set.erase(sets[index].set.begin());
            sets[index].set.emplace_back(cacheline(tag, true));
        }
    }

}

