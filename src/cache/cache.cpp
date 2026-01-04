#include "cache.h"


cacheline::cacheline(size_t _tag, bool _valid, bool _dirty)
    :valid(_valid), tag(_tag), dirty(_dirty){}

cacheset::cacheset(size_t _cap)
    :capacity(_cap){}


//linesize fixed at 8
cachelevel::cachelevel(size_t _lvl, size_t _size, size_t _assoc, std::string _repol )
    :lvl(_lvl), size(_size), linesize(8), assoc(_assoc), repol(_repol) 
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

    for (auto it = sets[index].set.begin(); it != sets[index].set.end(); ++it)
    {
        if(it->valid && it->tag == tag)
        {
            if(repol == "lru")
            {
                sets[index].set.erase(it);
                sets[index].set.emplace_back(cacheline(tag, true));
            }
            ++hits;
            return true;
        }
    }
    ++misses;
    return false;
}


void cachelevel::load(size_t addr)
{
    size_t blkaddress = (addr/linesize);
    size_t numlines = size/linesize;
    size_t numsets = numlines/assoc;
    size_t index = blkaddress % numsets;
    size_t tag = blkaddress / numsets;

    for (auto it = sets[index].set.begin(); it != sets[index].set.end();++it)
    {
        if(it->valid && it->tag == tag)
            return;
    }

    if (sets[index].set.size() >= sets[index].capacity)
        sets[index].set.erase(sets[index].set.begin());

    sets[index].set.emplace_back(cacheline(tag, true));
}

writeresult cachelevel::write(size_t addr)
{
    size_t blkaddress = (addr/linesize);
    size_t numlines = size/linesize;
    size_t numsets = numlines/assoc;
    size_t index = blkaddress % numsets;
    size_t tag = blkaddress / numsets;

    for(auto cline = sets[index].set.begin(); cline != sets[index].set.end(); ++cline)
    {
        if(cline->valid && cline->tag == tag)
        {
            cline->dirty = true;
            if(repol == "lru")
            {
                sets[index].set.erase(cline); 
                sets[index].set.emplace_back(cacheline(tag, true, true));
            }
            ++hits;
            return writeresult(false, 0, true);
        }
    }

    if(sets[index].set.size() >= sets[index].capacity)
    {
        if(sets[index].set.begin()->dirty)
        {
            size_t retaddr = sets[index].set.begin()->tag*numsets*linesize;
            sets[index].set.erase(sets[index].set.begin());
            sets[index].set.emplace_back(cacheline(tag, true, true));
            ++misses;
            return writeresult(true, retaddr, false);
        }
        else
        {
            sets[index].set.erase(sets[index].set.begin());
            sets[index].set.emplace_back(cacheline(tag, true, true));
            ++misses;
            return writeresult(false, 0, false);
        }
    }
    else
    {
        sets[index].set.emplace_back(cacheline(tag, true, true));
        ++misses;
        return writeresult(false, 0, false);
    }
}

void cachelevel::stats()
{
    std::cout << "Cache level L" << lvl << " has " << misses << " misses and " << hits << " hits" << std::endl;
    std::cout << "Cache hit ratio of level L" << lvl << " is: " << static_cast<double>(hits)/ (static_cast<double>(hits + misses)) << std::endl;

}


