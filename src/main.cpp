#include <iostream>
#include <sstream>
#include "allocator.h"
#include "cache.h"
#include "buddy.h"


class SystemSimulator {
private:
    const int LatencyL1{1};
    const int LatencyL2{10};
    const int Latencymem{100};

public:
    cachelevel l1;
    cachelevel l2;
    bool isBuddy;
    double totalcycles{};
    int reads{};
    int writes{};
    double hits{};
    double misses{};

    MemoryManager mem;

    BuddyManager buddymem;

    SystemSimulator(size_t memsize, std::string allocstrat, size_t cachesizel1, size_t cachesizel2, size_t cacheassoc, std::string cacheReplacementPolicy, size_t linesize, bool _isBuddy = false ) 
        : mem(MemoryManager(memsize, allocstrat)), l1(cachelevel(1, cachesizel1, cacheassoc, cacheReplacementPolicy, linesize)), l2(cachelevel(2, cachesizel2, cacheassoc, cacheReplacementPolicy, linesize)), buddymem(BuddyManager(memsize)) {}

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
            std::cout << "l2 cache hit\n" << std::endl;
            ++hits;
            std::cout << "address loaded to l1\n" << std::endl;
            l1.load(address);
            return;
        }

        totalcycles += Latencymem;

        ++misses;
        std::cout << "l1 and l2 miss" << std::endl;
        std::cout << "address loaded to l1 and l2\n" << std::endl; 
        l1.load(address);
        l2.load(address);
    }

    void write(size_t address)
    {
        writes++;

        if(!mem.isValidAddress(address))
        {
            std::cout << "segmentation fault\n" << std::endl;
            return;
        }        

        totalcycles += LatencyL1;
        writeresult l1ret = l1.write(address);

        if(!l1ret.wasHit)
        {
            ++misses;
            totalcycles += LatencyL2;
            if(!l2.access(address))
            {
                totalcycles += Latencymem;
                std::cout << "l1 and l2 miss for write\n";
                std::cout << "address loaded to l1\n";
            }
            else
            {
                std::cout << "l2 hit for write\n";
                std::cout << "address loaded to l1\n";
            }
        }
        else
        {
            std::cout << "l1 cache hit for write \n";
            ++hits;
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

void print_requirements()
{
    std::cout << std::endl;
    std::cout << "___Configuration must follow the following requirements___\n";
    std::cout << "- 0 < L1 size < L2 size < main memory size\n";
    std::cout << "- Buddy allocation requires memory size to be a power of two\n";
    std::cout << "- Line size must be a power of 2\n";
    std::cout << "- Cache size and main memory size must be multiple of line size\n";
    std::cout << "- Number of lines = L(1,2) size/linesize, must be divisible by associativity\n";
    std::cout << "- Associativity > 0\n";
    std::cout << "Note: if you want a quick run you can find test inputs in tests directory\n";
    std::cout << std::endl;
}

bool validateConfig(size_t memSize, size_t l1Size, size_t l2Size, size_t lineSize, size_t assoc, char cbuddy) 
{
    if (memSize <= 0 || l1Size <= 0 || l2Size <= 0 || lineSize <= 0 || assoc <= 0) 
    {
        std::cout << "Error: All sizes and associativity must be positive non-zero integers.\n";
        return false;
    }

    // Power of 2 Check

    if(cbuddy == 'y')
    {
        if((memSize & (memSize - 1)) != 0)
        {
            std::cout << "Error: memory size must be a power of 2 while using buddy allocator.\n";
            return false;
        }
    }
    if ((lineSize & (lineSize - 1)) != 0) 
    {
        std::cout << "Error: Line Size must be a power of 2 (e.g., 8, 16, 32, 64).\n";
        return false;
    }

    if (l1Size % lineSize != 0 || l2Size % lineSize != 0) 
    {
        std::cout << "Error: Cache sizes must be a multiple of the Line Size.\n";
        return false;
    }

    size_t l1Lines = l1Size / lineSize;
    size_t l2Lines = l2Size / lineSize;

    if (l1Lines % assoc != 0) 
    {
        std::cout << "Error: L1 Configuration invalid. Total lines (" << l1Lines 
                  << ") is not divisible by associativity (" << assoc << ").\n";
        return false;
    }
    if (l2Lines % assoc != 0) 
    {
        std::cout << "Error: L2 Configuration invalid. Total lines (" << l2Lines 
                  << ") is not divisible by associativity (" << assoc << ").\n";
        return false;
    }

    if (l1Size >= l2Size) 
    {
        std::cout << "Error: L1 Cache must be smaller than L2 Cache.\n";
        return false;
    }
    if (l2Size >= memSize) 
    {
        std::cout << "Error: L2 Cache must be smaller than Main Memory.\n";
        return false;
    }

    if (memSize % lineSize != 0) 
    {
        std::cout << "Warning: Main Memory size is not a multiple of Line Size. The last partial block may be unusable.\n";
    }

    return true;
}

int main()
{
    size_t memsize;
    std::string allocstrat;
    size_t l1size;
    size_t l2size;
    size_t cacheassoc;
    std::string cacheRePol;
    size_t linesize;
    char cbuddy;

    while(true)
    {
        print_requirements();
        std::cout << "Initialize MemoryAllocator and Cache\n";

        std::cout << "Do you want buddy allocation (N/y): ";
        std::cin >> cbuddy;

        std::cout << "Enter MainMemory size (bytes): ";
        std::cin >> memsize;

        if(cbuddy != 'y')
        {
            std::cout << "Enter Allocation Strategy (firstfit / bestfit / worstfit): ";
            std::cin >> allocstrat;
        }

        std::cout << "Enter L1 Cache Size (bytes): ";
        std::cin >> l1size;

        std::cout << "Enter L2 Cache Size (bytes): ";
        std::cin >> l2size;

        std::cout << "Enter linesize (bytes): ";
        std::cin >> linesize;     

        std::cout << "Enter Cache associativity ((1)direct associative / (2)-way associative / (4)-way associative / (8)-way associative): ";
        std::cin >> cacheassoc;

        std::cout << "Enter Cache Replacement Policy (fifo / lru): ";
        std::cin >> cacheRePol; 

        if(validateConfig(memsize, l1size, l2size, linesize, cacheassoc, cbuddy))
            break;
        else    
            std::cout << "Invalid configuration" << std::endl;
    }

    
    SystemSimulator proc(memsize, allocstrat, l1size, l2size, cacheassoc, cacheRePol, linesize);
    std::cout << "\n--simulator initialized successfully--\n";
    printhelp();

    if(cbuddy == 'y')
    {
        proc.isBuddy = true;
    }

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
            if(!proc.isBuddy)
            {
                std::cout << "---Allocator stats---\n";
                proc.mem.stats();
                std::cout << std::endl;
            }
        

            std::cout << "---Cache stats---\n";
            proc.l1.stats();
            proc.l2.stats();
            proc.stats();
        }

        else if(command == "memdump")
        {
            if(proc.isBuddy)
            {
                ;
            }
            else
            {
                proc.mem.dump();
            }
        }

        else if(command == "help")
        {
            printhelp();
        }

        else if (command == "malloc")
        {
            size_t size;

            if(ss >> size)
            {

                if(proc.isBuddy)
                {
                    proc.buddymem.malloc(size);
                }
                else
                    proc.mem.malloc(size);
            }
            else
                std::cout << "Error - Usage: malloc <size> - eg. malloc 1024\n";
        }

        else if (command == "free")
        {
            size_t id;

            if(ss >> id)
                if(proc.isBuddy)
                {
                    proc.buddymem.free(id);
                }
                else
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