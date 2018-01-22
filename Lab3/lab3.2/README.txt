CACHE参数：
L1：32768KB，8-way，64B block，hit latency: 3 cycles，bus latency: 0
L2：262144KB，8-way，64B block，hit latency: 4 cycles, bus latency: 6 cycles
memory：100 cycles
替换策略：ARC，预取：miss时取接下来的3行，旁路：基于当前miss rate

运行方法：
make clean;make进行编译
./sim <trace name>即可运行，需要输入运行轮数
result.txt中存有运行结果，access.txt中存有L1各组的访问次数和替换次数
