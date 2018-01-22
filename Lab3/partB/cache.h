#ifndef CACHE_CACHE_H_
#define CACHE_CACHE_H_

#include <stdint.h>
#include "storage.h"

//set结构体嵌套line结构体
typedef struct
{
    int valid;
    unsigned long tag;
    int last_access;
    int dirty;
    unsigned char *block;
}Line;
typedef struct
{
    Line *line;
}Set;


typedef struct CacheConfig_ {
    CacheConfig_(){}
    CacheConfig_(int a, int b, int c, int d, int e)
    {
        size = a;
        associativity = b;
        set_num = c;
        write_through = d;
        write_allocate = e;
        block_num = size / (set_num * associativity);
    }
    int size;
    int associativity;
    int set_num; // Number of cache sets
    int block_num; //number of blocks per line
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
} CacheConfig;

class Cache: public Storage {
    public:
    Cache() {}
    ~Cache()
    {
        delete set;
    }

    // Sets & Gets
    void SetConfig(CacheConfig cc);
    void GetConfig(CacheConfig cc);
    void SetLower(Storage *ll) { lower_ = ll; }
    // Main access process
    void HandleRequest(uint64_t addr, int bytes, int read,
                         char *content, int &hit, int &time);
    void pushBack(int &temp1, int &temp2);

    private:
      // Bypassing
    int BypassDecision();
      // Partitioning
    void PartitionAlgorithm();
      // Replacement
    int ReplaceDecision(unsigned long long addr, unsigned int &index);
    void ReplaceAlgorithm(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle);
      // Prefetching
    int PrefetchDecision();
    void PrefetchAlgorithm();

    int chooseVictim(int setPos);
    void updateLruCount(int setPos, int index);

    CacheConfig config_;
    Set *set;
    int access_count;
    Storage *lower_;
    DISALLOW_COPY_AND_ASSIGN(Cache);
};

#endif //CACHE_CACHE_H_
