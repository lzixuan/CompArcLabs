#include "cache.h"
#include "def.h"
#include<cstring>
#include<iostream>
#include<vector>
#include<cstdlib>
#include<time.h>

using namespace std;

extern double hit_rate1;
vector<unsigned long> bypassTable;
int tableSize = 8192;

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
void Cache::initArc()
{
    for (int i = 0; i < config_.set_num; i++)
    {
		vector<int> temp1;
		lru.push_back(temp1);
		vector<int> temp2;
		lfu.push_back(temp2);
		vector<unsigned long> temp3;
		lruGhost.push_back(temp3);
		vector<unsigned long> temp4;
		lfuGhost.push_back(temp4);
		lfuSize[i] = lruSize[i] = config_.associativity / 2;
    }
}
void Cache::printSet(FILE * res)
{
	int sum = 0;
	for (int i = 0; i < config_.set_num; i++)
	{
		fprintf(res, "set[%d]: access: %d replace: %d\n", i, set[i].accessCount, set[i].replace_num);
		sum += set[i].accessCount;
	}
	fprintf(res, "Total access: %d\n", sum);
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
		set[i].accessCount = 0;
		set[i].replace_num = 0;
    }
}
//content:写时为要写入内容，读时为读出内容
//prefetchFlag = 1: prefetch
void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle, int prefetchFlag) {
    if (prefetchFlag != 1)
    {
        access_count++;
        stats_.access_counter++;
		int s = log2(config_.set_num);
		int b = log2(config_.block_num);
		set[getSet(addr, s, b)].accessCount++;
    }
    // Bypass?
    if (!BypassDecision(addr))
    {
        PartitionAlgorithm();
        if (prefetchFlag != 1)
        {
            cycle += latency_.bus_latency + latency_.hit_latency;
            stats_.access_time += latency_.bus_latency + latency_.hit_latency;
        }
        // Miss?
        unsigned int index;
        int decision = ReplaceDecision(addr, index);
        if (decision == 1)
        {
            if (prefetchFlag != 1)
                stats_.miss_num++;
            // Choose victim
            ReplaceAlgorithm (addr, bytes, read, content, hit, cycle, prefetchFlag);
        }
        else
        {
            int s = log2(config_.set_num);
            int b = log2(config_.block_num);
            int setIndex = getSet(addr, s, b);
            int blockIndex = getBlock(addr, s, b);
            if (prefetchFlag != 1)
            {
                // return hit & time
                hit += 1;
                set[setIndex].line[index].last_access = access_count;
            }

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
					//stats_.fetch_num++;
					lower_->HandleRequest(addr, bytes, read, content, hit, cycle, prefetchFlag);
                }
            }
            if (read == 1)
            {
                memcpy(content, set[setIndex].line[index].block + blockIndex, bytes);
            }
        }
        // Prefetch?
        if (prefetchFlag != 1 && PrefetchDecision(addr, decision))
        {
            stats_.prefetch_num++;
            PrefetchAlgorithm(addr, decision);
        }
        return;
    }
	// Fetch from the lower layer
	// Add latency
	if (prefetchFlag != 1) {
		cycle += latency_.bus_latency;
		stats_.access_time += latency_.bus_latency;
		stats_.bypass_num++;
	}
	lower_->HandleRequest(addr, bytes, read, content, hit, cycle, prefetchFlag);
}

int Cache::BypassDecision(unsigned long addr) {
	/*if (stats_.access_counter <= 1000)
		return FALSE;
	int s = log2(config_.set_num);
	int b = log2(config_.block_num);
	unsigned long tag = getTag(addr, s, b);
	for (int i = 0; i < bypassTable.size(); i++)
	{
		if (tag == bypassTable[i])
			return FALSE;
	}
	return TRUE;*/
	if (stats_.access_counter <= 10000) {
		return FALSE;
	}
	if ((double)stats_.miss_num / (stats_.access_counter - stats_.bypass_num) >= 0.9)
    {
        srand(time(0));
        int temp = rand();
        if (temp % 2 == 0)
            return FALSE;
        else
            return TRUE;
    }
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
            for (int j = 0; j < lru[setIndex].size(); j++)
            {
                if (lru[setIndex][j] == i)
                {
					lru[setIndex].erase(lru[setIndex].begin() + j);
					lfu[setIndex].insert(lfu[setIndex].begin(), i);
                    if (lfu[setIndex].size() > lfuSize[setIndex])
                    {
                        int last = lfu[setIndex][lfu[setIndex].size() - 1];
                        unsigned long temp = set[setIndex].line[last].tag;
                        set[setIndex].line[last].valid = 0;
                        lfu[setIndex].pop_back();


                        lfuGhost[setIndex].insert(lfuGhost[setIndex].begin(), temp);
                        if (lfuGhost[setIndex].size() > 4)
                            lfuGhost[setIndex].pop_back();
                    }
                    return 0;
                }
            }
            for (int j = 0; j < lfu[setIndex].size(); j++)
            {
                if (lfu[setIndex][j] == i)
                {
					lfu[setIndex].erase(lfu[setIndex].begin() + j);
					lfu[setIndex].insert(lfu[setIndex].begin(), i);
                    return 0;
                }
            }
        }
    }
    for (int i = 0; i < lruGhost[setIndex].size(); i++)
    {
        if (lfuSize[setIndex] > 1)
        {
            if (lruGhost[setIndex][i] == tag)
            {
                lruSize[setIndex]++;
				lruGhost[setIndex].erase(lruGhost[setIndex].begin() + i);
				if (lfuSize[setIndex] == lfu[setIndex].size())
				{
					int last = lfu[setIndex][lfuSize[setIndex] - 1];
					unsigned long temp = set[setIndex].line[last].tag;
					set[setIndex].line[last].valid = 0;
					lfu[setIndex].pop_back();
					lfuGhost[setIndex].insert(lfuGhost[setIndex].begin(), temp);
					if (lfuGhost[setIndex].size() > 4)
						lfuGhost[setIndex].pop_back();
				}
                lfuSize[setIndex]--;


                return 1;
            }
        }
    }
    for (int i = 0; i < lfuGhost[setIndex].size(); i++)
    {
        if (lruSize[setIndex] > 1)
        {
            if (lfuGhost[setIndex][i] == tag)
            {
                lfuSize[setIndex]++;
				lfuGhost[setIndex].erase(lfuGhost[setIndex].begin() + i);
				if (lruSize[setIndex] == lru[setIndex].size())
				{
					int last = lru[setIndex][lruSize[setIndex] - 1];
					unsigned long temp = set[setIndex].line[last].tag;
					set[setIndex].line[last].valid = 0;
					lru[setIndex].pop_back();
					lruGhost[setIndex].insert(lruGhost[setIndex].begin(), temp);
					if (lruGhost[setIndex].size() > 4)
						lruGhost[setIndex].pop_back();
				}

                lruSize[setIndex]--;


                return 1;
            }
        }
    }
    return 1;
}

void Cache::ReplaceAlgorithm(uint64_t addr, int bytes, int read,
                             char *content, int &hit, int &cycle, int prefetchFlag)
{
    //not-write-allocate
    if (read == 0 && config_.write_allocate == 0)
    {
		//stats_.fetch_num++;
		lower_-> HandleRequest(addr, bytes, read, content, hit, cycle, prefetchFlag);
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
        lower_->HandleRequest(newAddr, config_.block_num, 0, (char *)victimLine.block, hit, cycle, prefetchFlag);
    }
    //request lower level to get the data
    unsigned long newAddr = addr - blockIndex;
    char *newBlock = (char *)victimLine.block;
    lower_->HandleRequest(newAddr, config_.block_num, 1, newBlock, hit, cycle, prefetchFlag);
    stats_.fetch_num++;

    if (prefetchFlag == 1)
        victimLine.prefetchFlag = 1;
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
    int index = -1;
    for (int i = 0; i < config_.associativity; i++)
    {
        if (set[setPos].line[i].valid == 0)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        if (lru[setPos].size() < lruSize[setPos])
        {
            lru[setPos].insert(lru[setPos].begin(), index);
        }
        else
        {
			int last = lru[setPos][lruSize[setPos] - 1];
            unsigned long temp = set[setPos].line[last].tag;
            lru[setPos].pop_back();
            lruGhost[setPos].insert(lruGhost[setPos].begin(), temp);
            if (lruGhost[setPos].size() > 4)
                lruGhost[setPos].pop_back();

			index = last;
			lru[setPos].insert(lru[setPos].begin(), index);
			stats_.replace_num++;
			set[setPos].replace_num++;
        }
    }
    else
    {
        int last = lru[setPos][lruSize[setPos] - 1];
        unsigned long temp = set[setPos].line[last].tag;
        lru[setPos].pop_back();
        lruGhost[setPos].insert(lruGhost[setPos].begin(), temp);
        if (lruGhost[setPos].size() > 4)
            lruGhost[setPos].pop_back();

        index = last;
        lru[setPos].insert(lru[setPos].begin(), index);
		stats_.replace_num++;
		set[setPos].replace_num++;
    }
    return index;
}
//LRU
/*int Cache::chooseVictim(int setPos)
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
    if (set[setPos].line[index].last_access != 0)
        stats_.replace_num++;
    return index;
}*/
void Cache::pushBack(int &temp1, int &temp2)
{
    int s = log2(config_.set_num);
    int b = log2(config_.block_num);
    for (int i = 0; i < config_.set_num; i++)
    {
        for (int j = 0; j < config_.associativity; j++)
        {
            if (set[i].line[j].dirty == 1)
            {
                unsigned long newAddr = (set[i].line[j].tag << (s + b)) + (i << b);
                lower_->HandleRequest(newAddr, config_.block_num, 0, (char *)set[i].line[j].block, temp1, temp2, 0);
            }
        }
    }
}

int Cache::PrefetchDecision(unsigned long addr, int miss)
{
    if (miss == 1)
        return 1;
    /*else
    {
        int s = log2(config_.set_num);
        int b = log2(config_.block_num);

        int setIndex = getSet(addr, s, b);
        unsigned int hitLine;
        ReplaceDecision(addr, hitLine);

        if (set[setIndex].line[hitLine].prefetchFlag == 1)
            return 1;
        else
            return 0;
    }*/
    return 0;
}

void Cache::PrefetchAlgorithm(unsigned long addr, int miss)
{
    char tmpContent[64];
    int tmp_hit = 0;
    int tmp_cycle = 0;

    for (int i = 1; i < config_.prefetchNum; i++)
        HandleRequest(addr + 64 * i, 0, 2, tmpContent, tmp_hit, tmp_cycle, 1);
    if (miss == 0 && config_.prefetchNum > 0)
        HandleRequest(addr + 64 * config_.prefetchNum, 0, 2, tmpContent, tmp_hit, tmp_cycle, 1);
}

