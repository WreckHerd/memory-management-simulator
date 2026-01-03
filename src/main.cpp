#include <iostream>
#include "allocator.h"
#include "cache.h"


class SystemSimulator
{
private:
    const int LatencyL1{1};
    const int LatencyL2{10};
    const int Latencymem{100};

public:
    cachelevel l1;
    cachelevel l2;
    double totalcycles{};
    int reads{};
    int writes{};

    MemoryManager mem;
    SystemSimulator(size_t memsize, std::string allocstrat, size_t cachesizel1, size_t cachesizel2) 
        : mem(MemoryManager(memsize, allocstrat)), l1(cachelevel(1, cachesizel1)), l2(cachelevel(1, cachesizel2)) {}

    void read(size_t address)
    {
        reads++;
        if(!mem.isValidAddress(address))
        {
            std::cout << "address is not valid" << std::endl;
            return; 
        }

        totalcycles += LatencyL1;
        if(l1.access(address))
        {
            std::cout << "l1 cache hit" << std::endl;
            return;
        }

        totalcycles += LatencyL2;
        if (l2.access(address))
        {
            std::cout << "l2 cache hit" << std::endl;
            l1.load(address);
            return;
        }

        totalcycles += Latencymem;

        std::cout << "l1 and l2 miss" << std::endl;
        l1.load(address);
        l2.load(address);
    }

    void write(size_t address)
    {
        writes++;

        if(!mem.isValidAddress(address))
        {
            std::cout << "Address is not valid" << std::endl;
            return;
        }        

        totalcycles += LatencyL1;
        writeresult l1ret = l1.write(address);

        if(!l1ret.wasHit)
        {
            totalcycles += LatencyL2;
            if(l2.access(address))
                totalcycles += Latencymem;
        }
        if(l1ret.wasEvicted)
        {
            totalcycles += LatencyL2;
            writeresult l2ret = l2.write(address);

            std::cout << "L1 Eviction: writing dirty block " << l1ret.address << " to l2" << std::endl;

            if(l2ret.wasEvicted)
            {
                totalcycles += Latencymem;
                std::cout << "L2 Eviction : writing dirty block " << l2ret.address << " to main memory" << std:: endl;
            }
        }

    }

    void stats()
    {
        long totaloperations = reads + writes;
        if(totaloperations > 0)
        {
            double amat = totalcycles / totaloperations;
            std::cout << "Average Memory access time: " << amat << "cycles"<< std::endl;
        }
        else
            std::cout << "no reads or writes performed yet";
    }
};

int main()
{
    SystemSimulator proc(1024, "worstfit", 64, 512);
    proc.read(500);
    proc.read(1030);
    proc.read(500);

    std::cout << "///" << std::endl;

    proc.read(64);
    proc.read(0);
    proc.read(64);
    proc.read(32);

    proc.read(64);
    proc.read(0);




    proc.mem.dump();
    std::cout << "///" << std::endl;

    proc.mem.malloc(100);
    proc.mem.malloc(200);
    proc.mem.malloc(300);
    
    proc.mem.dump();
    std::cout << "///" << std::endl;

    proc.mem.free(2);

    proc.mem.dump();
    std::cout << "///" << std::endl;
    
    proc.mem.malloc(50);

    proc.mem.dump();

    std::cout << "///" << std::endl;

    proc.mem.stats();

    std::cout << "///" << std::endl;
    proc.l1.stats();
    proc.l2.stats();

    std::cout << "///" << std::endl;

    proc.stats();

}        