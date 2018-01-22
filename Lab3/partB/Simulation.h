#ifndef RISCV
#define RISCV


#include"Reg_def.h"
#include <stdint.h>
#include "storage.h"
#include<cstring>
#include<string>

//simulation define
#define OP_JAL 111
#define OP_R 51

#define OP_JALR 103
#define OP_LUI 55
#define OP_AUIPC 23


#define F3_ADD 0
#define F3_MUL 0

#define F7_MSE 1
#define F7_ADD 0

#define OP_I 19
#define F3_ADDI 0

#define OP_SW 35
#define F3_SB 0

#define OP_LW 3
#define F3_LB 0

#define OP_BEQ 99
#define F3_BEQ 0

#define OP_IW 27
#define F3_ADDIW 0

#define OP_RW 59
#define F3_ADDW 0
#define F7_ADDW 0


#define OP_SCALL 115
#define F3_SCALL 0
#define F7_SCALL 0

#define MAX (1 << 28)

//寄存器堆
REG reg[32]={0};
//PC
long long PC=0;


//各个指令解析段
unsigned int OP=0;
unsigned int func3=0,func7=0, func7_s = 0;
int shamt=0;
int rs1=0,rs2=0,rd=0;
unsigned int imm12=0;
unsigned int imm20=0;
unsigned int imm7=0;
unsigned int imm5=0;

//各阶段
void simulate();
void IF();
void ID();
void EX();
void MEM();
void WB();

//主存
class Memory:public Storage
{

public:
    char mem_space[MAX];
    Memory()
    {
        memset(mem_space, 0, sizeof(mem_space));
    }
    //检查下标
    unsigned int get32(long long addr)
    {
        if (!checkSub(addr))
            return *(unsigned int*)(mem_space+addr);
    }
    int checkSub(int sub)
    {
        if (sub < 0 || sub > MAX - 4)
        {
            printf("segmentation fault: access(0x%x) is out of range\r\n", sub);
            exit(0);
        }
        return 0;
    }
    char *getAddress(long long pc)
    {
        return mem_space + pc;
    }
    char * operator + (long long offset)
    {
        return mem_space + offset;
    }
    void HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle)
    {
        cycle += latency_.hit_latency + latency_.bus_latency;
        stats_.access_time += latency_.hit_latency + latency_.bus_latency;
        stats_.access_counter++;
        if (read == 0)
        {
            memcpy(mem_space + addr, content, bytes);
        }
        else
        {
            memcpy(content, mem_space + addr, bytes);
        }
    }
private:
  // Memory implement
  DISALLOW_COPY_AND_ASSIGN(Memory);
};

//符号扩展
int ext_signed(unsigned int src,int bit);

//获取指定位
unsigned int getbit(int s,int e);

int ext_signed(unsigned int src, int bit)
{
    if (bit)
    {
        int temp = (int)src;
        return ((temp << (32 - bit)) >> (32 - bit));
    }
    else
        return src;
}

unsigned int getbit(unsigned inst,int s,int e)
{
	int tmp = 0;
	for (int i = s; i <= e; i++)
        tmp += (1 << i);
    int res = ((tmp & inst) >> s);
	return res;
}


#endif


