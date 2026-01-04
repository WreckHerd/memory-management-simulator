#include <iostream>
#include <sstream>
#include "allocator.h"
#include "cache.h"


class SystemSimulator {
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
    double hits{};
    double misses{};

    MemoryManager mem;

    SystemSimulator(size_t memsize, std::string allocstrat, size_t cachesizel1, size_t cachesizel2, size_t cacheassoc, std::string cacheReplacementPolicy) 
        : mem(MemoryManager(memsize, allocstrat)), l1(cachelevel(1, cachesizel1, cacheassoc, cacheReplacementPolicy)), l2(cachelevel(2, cachesizel2, cacheassoc, cacheReplacementPolicy)) {}

    void read(size_t address)
    {
        reads++;
        if(!mem.isValidAddress(address))
        {
            std::cout << "segmentation fault" << std::endl;
            return; 
        }

        totalcycles += LatencyL1;
        if(l1.access(address))
        {
            std::cout << "l1 cache hit" << std::endl;
            ++hits;
            return;
        }

        totalcycles += LatencyL2;
        if (l2.access(address))
        {
            std::cout << "l2 cache hit" << std::endl;
            ++hits;
            l1.load(address);
            return;
        }

        totalcycles += Latencymem;

        ++misses;
        std::cout << "l1 and l2 miss" << std::endl;
        l1.load(address);
        l2.load(address);
    }

    void write(size_t address)
    {
        writes++;

        if(!mem.isValidAddress(address))
        {
            std::cout << "segmentation fault" << std::endl;
            return;
        }        

        totalcycles += LatencyL1;
        writeresult l1ret = l1.write(address);

        if(!l1ret.wasHit)
        {
            ++misses;
            totalcycles += LatencyL2;
            if(!l2.access(address))
                totalcycles += Latencymem;
        }
        else
            ++misses;

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
        double hitratio = hits/(hits+misses);

        std::cout << "Hit ratio: " << hitratio << std::endl;

        long totaloperations = reads + writes;
        if(totaloperations > 0)
        {
            double amat = totalcycles / totaloperations;
            std::cout << "Average Memory access time: " << amat << " cycles"<< std::endl;
        }
        else
            std::cout << "no reads or writes performed yet";

    }
};

void printhelp()
{
    std::cout << "\n---List of commands---\n";
    std::cout << "malloc <size>     : allaocate a block of <size> in memory\n";
    std::cout << "free <blockId>    : free the block with id <blockId\n"; 
    std::cout << "memdump           : print the current state of memeory\n";
    std::cout << std::endl;
    std::cout << "read <address>    : read data a <address> from cache or memory\n";
    std::cout << "write <address>   : write into <address>\n";
    std::cout << "stats             : print stats related to cache and memory\n";
    std::cout << "quit / exit       : exit the simulator\n";
    std::cout << "-------------------------\n";
}

int main()
{
    size_t memsize;
    std::string allocstrat;
    size_t l1size;
    size_t l2size;
    size_t cacheassoc;
    std::string cacheRePol;

    std::cout << "Initialize MemoryAllocator and Cache\n";

    std::cout << "Enter MainMemory size (bytes): ";
    std::cin >> memsize;

    std::cout << "Enter Allocation Strategy (firstfit / bestfit / worstfit): ";
    std::cin >> allocstrat;

    std::cout << "Enter L1 Cache Size (bytes): ";
    std::cin >> l1size;

    std::cout << "Enter L2 Cache Size (bytes): ";
    std::cin >> l2size;

    std::cout << "Enter Cache associativity ((1)direct associative / (2)-way associative / (4)-way associative / (8)-way associative): ";
    std::cin >> cacheassoc;

    std::cout << "Enter Cache Replacement Policy (fifo / lru): ";
    std::cin >> cacheRePol; 
    
    SystemSimulator proc(memsize, allocstrat, l1size, l2size, cacheassoc, cacheRePol);
    std::cout << "\n--simulator initialized successfully--\n";
    printhelp();

    std::string dummy;
    getline(std::cin, dummy);

    std::string line;
    while(true)
    {
        std::cout << "\n MEMSIM> ";
        if(!std::getline(std::cin, line)) 
            break;
        if(line.empty())
            continue;
        std::stringstream ss(line);

        std::string command;

        ss >> command;

        if(command == "exit" || command == "quit")
            break;

        else if (command == "stats")
        {
            std::cout << "---Allocator stats---\n";
            proc.mem.stats();
            std::cout << std::endl;
        

            std::cout << "---Cache stats---\n";
            proc.l1.stats();
            proc.l2.stats();
            proc.stats();
        }

        else if(command == "memdump")
        {
            proc.mem.dump();
        }

        else if(command == "help")
        {
            printhelp();
        }

        else if (command == "malloc")
        {
            size_t size;

            if(ss >> size)
                proc.mem.malloc(size);
            else
                std::cout << "Error - Usage: malloc <size> - eg. malloc 1024\n";
        }

        else if (command == "free")
        {
            size_t id;

            if(ss >> id)
                proc.mem.free(id);
            else    
                std::cout << "Error - Usage: free <id>\n";
        }

        else if (command == "read") 
        {
            size_t addr;

            if (ss >> addr) 
                proc.read(addr);
            else
                std::cout << "Error: Usage: read <address>\n";
        }

        else if (command == "write") 
        {
            size_t addr;

            if (ss >> addr)
                proc.write(addr);
            else
                std::cout << "Error: Usage: write <address>\n";
        }

        else
        {
            std::cout << "Unknown command " << command << ". Type 'help' for command list\n";
        }
    }

    std::cout << "Simulation Terminated\n";
    return 0;

}        