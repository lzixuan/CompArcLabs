#ifndef CACHE_STORAGE_H_
#define CACHE_STORAGE_H_

#include<stdint.h>
#include<stdio.h>
#include<iostream>

using namespace std;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

// Storage access stats
typedef struct StorageStats_ {
  int access_counter;
  int miss_num;
  int access_time; // In nanoseconds
  int replace_num; // Evict old lines
  int fetch_num; // Fetch lower layer
  int prefetch_num; // Prefetch
  int bypass_num;//bypass
} StorageStats;

// Storage basic config
typedef struct StorageLatency_ {
  int hit_latency; // In nanoseconds
  int bus_latency; // Added to each request
} StorageLatency;

    class Storage {
    public:
    Storage() {}
    ~Storage() {}
	StorageStats stats_;
	StorageLatency latency_;
    // Sets & Gets
    void SetStats(StorageStats ss)
    {
        stats_ = ss;
        stats_.access_counter = 0;
        stats_.miss_num = 0;
        stats_.replace_num = 0;
        stats_.fetch_num = 0;
        stats_.prefetch_num = 0;
		stats_.bypass_num = 0;
    }
    void GetStats(StorageStats &ss) { ss = stats_; }
    void SetLatency(StorageLatency sl) { latency_ = sl; }
    void GetLatency(StorageLatency &sl) { sl = latency_; }
    void printStats(FILE* res, double &missrate, double &bypassrate)
    {
        printf("access count: %d\n", stats_.access_counter);
        fprintf(res, "access count: %d\n", stats_.access_counter);
        printf("access time: %d\n", stats_.access_time);
        fprintf(res, "access time: %d\n", stats_.access_time);
        printf("miss num: %d\n", stats_.miss_num);
        fprintf(res, "miss num: %d\n", stats_.miss_num);
		missrate = (double)stats_.miss_num / (stats_.access_counter - stats_.bypass_num);
		bypassrate = (double)stats_.bypass_num / stats_.access_counter;
        printf("miss rate: %lf\n", (double)stats_.miss_num / (stats_.access_counter - stats_.bypass_num));
        fprintf(res, "miss rate: %lf\n", (double)stats_.miss_num / (stats_.access_counter - stats_.bypass_num));
        printf("replace_num(prefetch included): %d\n", stats_.replace_num);
        fprintf(res, "replace_num(prefetch included): %d\n", stats_.replace_num);
        printf("prefetch_num: %d\n", stats_.prefetch_num);
        fprintf(res, "prefetch_num: %d\n", stats_.prefetch_num);
        printf("fetch from the lower layer: %d\n", stats_.fetch_num);
        fprintf(res, "fetch from the lower layer: %d\n", stats_.fetch_num);
		printf("bypass current level: %d\n", stats_.bypass_num);
		fprintf(res, "bypass current level: %d\n", stats_.bypass_num);
    }

  // Main access process
  // [in]  addr: access address
  // [in]  bytes: target number of bytes
  // [in]  read: 0|1 for write|read
  // [i|o] content: in|out data
  // [out] hit: 0|1 for miss|hit
  // [out] time: total access time
  virtual void HandleRequest(uint64_t addr, int bytes, int read,
                             char *content, int &hit, int &time, int prefetchFlag) = 0;

 protected:
};

#endif //CACHE_STORAGE_H_
