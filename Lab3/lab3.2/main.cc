#include "stdio.h"
#include "cache.h"
#include "memory.h"
#include<fstream>
#include<iostream>

using namespace std;

double hit_rate1;
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
    Cache l1, l2;

    string file(argv[1]);
    //default latency
	StorageLatency ml;
    ml.bus_latency = 0;
    ml.hit_latency = 100;
    m.SetLatency(ml);


    StorageLatency ll;
    ll.bus_latency = 0;
    ll.hit_latency = 3;
    l1.SetLatency(ll);
    ll.bus_latency = 6;
    ll.hit_latency = 4;
    l2.SetLatency(ll);

    //initialize
	CacheConfig stat1(32768, 8, 64, 0, 1);
	CacheConfig stat2(262144, 8, 512, 0, 1);


    l1.SetLower(&l2);
    l2.SetLower(&m);
    l1.SetConfig(stat1);
    l2.SetConfig(stat2);


    StorageStats s;
    s.access_time = 0;
    m.SetStats(s);
    l1.SetStats(s);
    l2.SetStats(s);

    l1.initArc();
	l2.initArc();
    int hit = 0, cycle = 0, count = 0;
    char content[64];
    unsigned long long addr;
    char method;
    ifstream fin;
    int round;
    cout << "please input the number of rounds: ";
    cin >> round;
    cout << endl;
	for (int i = 0; i < round; i++)
	{
		fin.open(argv[1]);
		//deal the requests in the trace file
		while (fin >> method)
		{
			count++;
			fin >> hex >> addr;
			if (method == 'w')
				l1.HandleRequest(addr, 0, 0, content, hit, cycle, 0);
			else
				l1.HandleRequest(addr, 0, 1, content, hit, cycle, 0);
            hit_rate1 = (double)l1.stats_.miss_num / (l1.stats_.access_counter - l1.stats_.bypass_num);
		}
		fin.close();
	}
	double missR1, missR2, temp1, bypass1, bypass2, temp2;
    FILE *res = fopen("result.txt", "w");
    printf("\n");
	fprintf(res, "L1------------------------------\n");
	cout << "L1------------------------------" << endl;
    l1.printStats(res, missR1, bypass1);
	fprintf(res, "L2------------------------------\n");
	cout << "L2------------------------------" << endl;
    l2.printStats(res, missR2, bypass2);
	fprintf(res, "memory------------------------------\n");
	cout << "memory------------------------------" << endl;
    m.printStats(res, temp1, temp2);
	FILE * res2 = fopen("access.txt", "w");
	l1.printSet(res2);
	fclose(res2);
	printf("\n");
	fprintf(res, "\n");
	fprintf(res, "Access Count: : %d\n", count);
    cout << "Access Count: " << count << endl;
	fprintf(res, "Total Access Time(in cycles, prefetch time not included): %d\n", cycle);
    cout << "Total Access Time(in cycles, prefetch time not included): " << cycle << endl;
	double amat1, amat2;
	missR1 = missR1 * (1 - bypass1);
	missR2 = missR2 * (1 - bypass2);
	amat1 = 3 * (1 - bypass1) + 6 * (bypass1 + missR1) + 4 * (missR1 + bypass1) * (1 - bypass2)
				  + 100 * (missR1 * missR2 + missR1 * bypass2 + bypass1 * missR2 + bypass1 * bypass2);
	amat2 = (double)cycle / count;
	cout << "AMAT1(according to miss rate):" << amat1 << endl;
	fprintf(res, "amat1: %lf\n", amat1);
	cout << "AMAT2(according to total cycles):" << amat2 << endl;
	fprintf(res, "amat2: %lf\n", amat2);
    fclose(res);
    return 0;
}
