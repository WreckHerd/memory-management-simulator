#include <iostream>
#include "allocator.h"
#include "cache.h"


class SystemSimulator
{
private:
    cachelevel l1;
    cachelevel l2;

public:
    MemoryManager mem;
    SystemSimulator(size_t memsize, std::string allocstrat, size_t cachesizel1, size_t cachesizel2) 
        : mem(MemoryManager(memsize, allocstrat)), l1(cachelevel(1, cachesizel1)), l2(cachelevel(1, cachesizel2)) {}

    void read(size_t address)
    {
        if(!mem.isValidAddress(address))
        {
            std::cout << "address is not valid" << std::endl;
            return; 
        }

        if(l1.access(address))
        {
            std::cout << "l1 cache hit" << std::endl;
        }
        else if (l2.access(address))
        {
            std::cout << "l2 cache hit" << std::endl;
            l1.load(address);
        }
        else
        {
            std::cout << "l1 and l2 miss" << std::endl;
            l1.load(address);
            l2.load(address);
        }

    }
};

int main()
{
    SystemSimulator proc(1024, "worstfit", 100, 200);
    proc.read(500);
    proc.read(1030);
    proc.read(500);

    std::cout << "///" << std::endl;

    proc.read(16);
    proc.read(0);
    proc.read(32);

    proc.read(0);
    proc.read(16);




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
}        