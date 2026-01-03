#include "cache.h"


cacheline::cacheline(size_t _tag, bool _valid)
    :valid(_valid), tag(_tag){}

cacheset::cacheset(size_t _cap)
    :capacity(_cap){}


//linesize fixed at 8
cachelevel::cachelevel(size_t _lvl, size_t _size, size_t _assoc, std::string _repol )
    :lvl(_lvl), size(_size), linesize(8), /*lines(std::vector<cacheline>(_size/linesize))*/ assoc(_assoc), repol(_repol) 
    {   
        size_t numlines = size/linesize;
        size_t numsets = numlines/assoc;

        for (int i {}; i < numsets; ++i)
        {
            sets.emplace_back(assoc);
        }
    }

bool cachelevel::access(size_t addr)
{
    size_t blkaddress = (addr/linesize);
    size_t numlines = size/linesize;
    size_t numsets = numlines/assoc;
    size_t index = blkaddress % numsets;
    size_t tag = blkaddress / numsets;

    if(assoc == 1)
    {
        if(sets[index].set.empty())
            return false;
        else
        {
            if(sets[index].set[0].tag == tag && sets[index].set[0].valid)
                return true;
            else
                return false;
        }
    }
    else 
    {
        if(repol == "fifo")
        {
            for (auto it = sets[index].set.begin(); it != sets[index].set.end(); ++it)
            {
                if(it->valid && it->tag == tag)
                    return true;
            }

            return false;
        }

        else if (repol == "lru")
        {
            for (auto it = sets[index].set.begin(); it != sets[index].set.end(); ++it)
            {
                if(it->valid && it->tag == tag)
                {
                    sets[index].set.erase(it);
                    sets[index].set.emplace_back(cacheline(tag, true));
                    return true;
                }
            }

            return false;

        }

    }

    return false;
}

void cachelevel::load(size_t addr)
{
    size_t blkaddress = (addr/linesize);
    size_t numlines = size/linesize;
    size_t numsets = numlines/assoc;
    size_t index = blkaddress % numsets;
    size_t tag = blkaddress / numsets;

    if(assoc == 1)
    {
        if(sets[index].set.empty())
        {
            sets[index].set.emplace_back(cacheline(tag, true));
        }
        else
        {
            sets[index].set[0].tag = tag;
            sets[index].set[0].valid = true;
        }

    }

    else 
    {
        if(repol == "fifo")
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

        else if (repol == "lru")
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

