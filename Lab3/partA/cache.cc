#include "cache.h"
#include "def.h"
#include<cstring>
#include<iostream>

using namespace std;

int log2(unsigned long x)
{
    int n = -1;
    while (x)
    {
        x >>= 1;
        n++;
    }
    return n;
}
//获取地址对应的行号
int getSet(unsigned long addr, int s, int b)
{
    addr >>= b;
    int mask = (1 << s) - 1;
    return (addr & mask);
}
//获取地址对应的标签
unsigned long getTag(unsigned long addr, int s, int b)
{
    return (addr >> (s + b));
}
//获取块号
int getBlock(unsigned long addr, int s, int b)
{
    int mask = (1 << b) - 1;
    return (addr & mask);
}

//initialize the cache
void Cache::SetConfig(CacheConfig cc)
{
    config_ = cc;
    access_count = 0;
    set = new Set[config_.set_num];
    for (int i = 0; i < config_.set_num; i++)
    {
        set[i].line = new Line[config_.associativity];
        memset(set[i].line, 0, config_.associativity * sizeof(Line));
        for (int j = 0; j < config_.associativity; j++)
        {
            set[i].line[j].block = new unsigned char[config_.block_num];
        }
    }
}
//content:写时为要写入内容，读时为读出内容
void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle) {
    access_count++;
    // Bypass?
    if (!BypassDecision())
    {
        PartitionAlgorithm();
        // Miss?
        unsigned int index;
        int decision = ReplaceDecision(addr, index);
        cycle += latency_.bus_latency + latency_.hit_latency;
        stats_.access_time += latency_.bus_latency + latency_.hit_latency;
        if (decision == 1)
        {
            // Choose victim
            ReplaceAlgorithm (addr, bytes, read, content, hit, cycle);
        }
        else
        {
            // return hit & time
            hit += 1;
            
            int s = log2(config_.set_num);
            int b = log2(config_.block_num);
            int setIndex = getSet(addr, s, b);
            int blockIndex = getBlock(addr, s, b);
            set[setIndex].line[index].last_access = access_count;
            //write
            if (read == 0)
            {
                memcpy(set[setIndex].line[index].block + blockIndex, content, bytes);
                //写回
                if (config_.write_through == 0){
                    set[setIndex].line[index].dirty = 1;
                }
                //直写
                else
                {
                    lower_->HandleRequest(addr, bytes, read, content, hit, cycle);
                }
            }
            else
            {
                memcpy(content, set[setIndex].line[index].block + blockIndex, bytes);
            }
            return;
        }
    }
    // Prefetch?
    if (PrefetchDecision())
    {
        PrefetchAlgorithm();
    }
    else
    {
        // Fetch from lower layer
    }
}

int Cache::BypassDecision() {
  return FALSE;
}

void Cache::PartitionAlgorithm() {
}

int Cache::ReplaceDecision(unsigned long long addr, unsigned int &index) {
    int s = log2(config_.set_num);
    int b = log2(config_.block_num);
    unsigned long tag = getTag(addr, s, b);
    int setIndex = getSet(addr, s, b);
    //check if it is hit
    for (int i = 0; i < config_.associativity; i++)
    {
        if (set[setIndex].line[i].tag == tag && set[setIndex].line[i].valid == 1)
        {
            index = i;
            return 0;
        }
    }
    return 1;
}

void Cache::ReplaceAlgorithm(uint64_t addr, int bytes, int read,
                             char *content, int &hit, int &cycle)
{
    //not-write-allocate
    if (read == 0 && config_.write_allocate == 0)
    {
        lower_-> HandleRequest(addr, bytes, read, content, hit, cycle);
        return;
    }
    int s = log2(config_.set_num);
    int b = log2(config_.block_num);
    unsigned long tag = getTag(addr, s, b);
    int setIndex = getSet(addr, s, b);
    int blockIndex = getBlock(addr, s, b);
    //choose the line to replace
    int victim = chooseVictim(setIndex);
    Line &victimLine = set[setIndex].line[victim];
    //dirty = 1 -> write to the lower level first
    if (victimLine.dirty == 1)
    {
        unsigned long newAddr = (victimLine.tag << (s + b)) + (setIndex << b);
        lower_->HandleRequest(newAddr, config_.block_num, 0, (char *)victimLine.block, hit, cycle);
    }
    //request lower level to get the data
    unsigned long newAddr = addr - blockIndex;
    char *newBlock = (char *)victimLine.block;
    lower_->HandleRequest(newAddr, config_.block_num, 1, newBlock, hit, cycle);

    victimLine.valid = 1;
    victimLine.tag = tag;
    victimLine.last_access = access_count;
    //write-allocate
    if (read == 0)
    {
        victimLine.dirty = 1;
        for (int i = 0; i < bytes; i++)
            victimLine.block[i + blockIndex] = content[i];
    }
    else
    {
        //reset the dirty bit
        victimLine.dirty = 0;
        for (int i = 0; i < bytes; i++)
            content[i] = victimLine.block[blockIndex + i];
    }
}
//选择被替换的行
int Cache::chooseVictim(int setPos)
{
    int minCount = 1000000, index = 0;
    for (int i = 0; i < config_.associativity; i++)
    {
        if (set[setPos].line[i].last_access < minCount)
        {
            minCount = set[setPos].line[i].last_access;
            index = i;
        }
    }
    return index;
}

int Cache::PrefetchDecision() {
  return FALSE;
}

void Cache::PrefetchAlgorithm() {
}

