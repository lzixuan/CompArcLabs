#include "stdio.h"
#include "cache.h"
#include "memory.h"
#include<fstream>
#include<iostream>

using namespace std;

int main(int argc, char *argv[]) {
    //input the trace file name
    if (argc < 2)
    {
        cout << "argument lost!" <<endl;
        return 0;
    }
    else if (argc > 2)
    {
        cout << "too many arguments!" << endl;
        return 0;
    }

    Memory m;
    Cache l1;

    string file(argv[1]);
	int size;				// Cache's size (in bytes)
	int associativity;		// associativity of the cache
	int block_size;
	int hitLatency;
	int set_num;			// Number of cache sets
	int write_through;		// 0|1 for back|through
	int write_allocate;		// 0|1 for no-alc|alc
    //default latency
	StorageLatency ml;
    ml.bus_latency = 0;
    ml.hit_latency = 100;


    StorageLatency ll;
    ll.bus_latency = 0;
    ll.hit_latency = 10;


    //input the cache parameters
	cout << "Size of cache(in byte): ";
	cin >> size;
	cout << "Associativity: ";
	cin >> associativity;
	cout << "Block size(in byte): ";
	cin >> block_size;
	set_num = size / (associativity * block_size);
	cout << "Write hit policy(0|1 for back|through): ";
	cin >> write_through;
	cout << "Write missing policy(0|1 for no-alc|alc): ";
	cin >> write_allocate;
	cout << "Cache hit latency (in cycles): ";
	cin >> hitLatency;
	ll.hit_latency = hitLatency;
    //initialize
	CacheConfig stat(size, associativity, set_num, write_through, write_allocate);

    l1.SetLatency(ll);
    m.SetLatency(ml);
    l1.SetLower(&m);
    l1.SetConfig(stat);

    StorageStats s;
    s.access_time = 0;
    m.SetStats(s);
    l1.SetStats(s);

    m.GetStats(s);


    int hit = 0, cycle = 0, count = 0;
    char content[64];
    unsigned long long addr;
    char method;
    ifstream fin;
    fin.open(argv[1]);
    //deal the requests in the trace file
    while (fin >> method >> addr)
    {
        count++;
        if (method == 'w')
            l1.HandleRequest(addr, 0, 0, content, hit, cycle);
        else
            l1.HandleRequest(addr, 0, 1, content, hit, cycle);
    }

    printf("\n");
    l1.GetStats(s);
    printf("Total L1 access time: %dcycles\n", s.access_time);
    m.GetStats(s);
    printf("Total Memory access time: %dcycles\n", s.access_time);
    cout << "Hit times: " << hit << endl;
    cout << "Access Count: " << count << endl;
    cout << "Hit Rate: " << ((double)hit / (double)count) << endl;
    cout << "Miss Rate: " << 1 - ((double)hit / (double)count) << endl;
    cout << "Total Access Time(in cycles): " << cycle << endl;
    fin.close();
    return 0;
}
