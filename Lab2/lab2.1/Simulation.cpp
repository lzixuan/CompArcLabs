#include<cstdio>
#include<cstring>
#include<iostream>
#include<cmath>
//#include <io.h>
//#include <process.h>
#include<time.h>
#include<stdlib.h>
#include "Simulation.h"
#include "Read_Elf.h"

//#define test
//#define test2
//加减乘除：0-3.异或：4,与或非：5-7,左移右移：8-9
//高位乘：10,比大小：11,取余：12,算术右移：13
//判等：14 不等：15 大于：16 addiw：18 lui:19 auipc:20
#define add 0
#define sub 1
#define mul 2
#define div 3
#define xor2 4
#define and2 5
#define or2 6
#define not2 7
#define sll 8
#define srl 9
#define mulh 10
#define lt 11
#define mod 12
#define sra 13
#define equ 14
#define nequ 15
#define ge 16
#define upPC 17
#define addiw 18
#define lui 19
#define auipc 20
#define addw 21
#define mulw 22
#define subw 23
#define divw 24
#define modw 25
#define B_IMM(x) (((x&(1<<31))>>19)|((x>>7&1)<<11)|(((x>>25)&63)<<5)|(((x>>8)&15)<<1))
#define J_IMM(x) (((x&(1<<31))>>11)|(((x>>12)&255)<<12)|(((x>>20)&1)<<11)|(((x>>25)&63)<<5)|(((x>>21)&15)<<1))
#define S_IMM(x)	(((x&(1<<31))>>20)|(((x>>25)&63)<<5)|(((x>>8)&15)<<1)|(x>>7&1))



using namespace std;

FILE *elf = NULL;
Elf64_Ehdr elf64_hdr;

//regNames
char i_reg_name[32][5] = {	"zero", "ra  ", "sp  ", "gp  ",
				"tp  ", "t0  ", "t1  ", "t2  ",
				"s0  ", "s1  ", "a0  ", "a1  ",
				"a2  ", "a3  ", "a4  ", "a5  ",
				"a6  ", "a7  ", "s2  ", "s3  ",
				"s4  ", "s5  ", "s6  ", "s7  ",
				"s8  ", "s9  ", "s10 ", "s11 ",
				"t3  ", "t4  ", "t5  ", "t6  "	};

//Program headers
unsigned long long padr=0;
unsigned int psize=0;
unsigned int pnum=0;

//Section Headers
unsigned long long sadr=0;
unsigned int ssize=0;
unsigned int snum=0;

//Symbol table
unsigned int symnum=0;
unsigned long long symadr=0;
unsigned int symsize=0;

//用于指示 包含节名称的字符串是第几个节（从零开始计数）
unsigned int shstrindex=0;
unsigned int shstradr=0;

//字符串表在文件中地址，其内容包括.symtab和.debug节中的符号表
unsigned int stradr=0;

extern unsigned int cadr;
extern unsigned int csize;
extern unsigned int vadr;
extern unsigned long long gp;
extern unsigned int madr;
extern unsigned long long endPC;
extern unsigned int entry;

extern FILE *file;

Memory memory;

//指令运行数
long long inst_num=0;

//系统调用退出指示
int exit_flag=0;
int step_flag = 0;

//加载代码段,use load_program headers instead
/*void load_memory()
{
	fseek(file,cadr,SEEK_SET);
	fread(memory.getAddress(vadr),1,csize,file);

	vadr=vadr>>2;
	csize=csize>>2;
	fclose(file);
}*/
void checkReg()
{
    for (int i = 0; i < 8; i++)
	{
        for (int j = 0; j < 4; j++)
         {
            printf("%4s: 0x%llx  ", i_reg_name[i * 4 + j], reg[i * 4 + j]);
         }
         cout << endl;
	}
}
int main(int argc, char *argv[])
{
	if (argc == 1)
	{
        printf("please input an elf file!\n");
        exit(0);
	}
	if (argc > 3)
	{
        printf("Oops, too many parameters!\n");
        exit(0);
	}

	if (argc == 3)
	{
        if (strcmp("-s", argv[2]) != 0)
        {
            printf("illegal parameter!(step mode: -s)\n");
            exit(0);
        }
        else
        {
            step_flag = 1;
        }
	}
	memset(reg, 0, sizeof(reg));
	//解析elf文件
	read_elf(argv[1]);

	//加载内存
	if (!load_program())
	{
        printf("segment error\n");
        exit(0);
	}

	//设置入口地址
	//PC=entry;

	//设置全局数据段地址寄存器
	reg[3]=gp;

	reg[2]=MAX/2;//栈基址 （sp寄存器）

	simulate();

	cout <<"simulate over!"<<endl;

	return 0;
}



void simulate()
{
	//int instCnt = 0;
	//结束PC的设置
	while(PC != endPC)
	{
		//运行
		IF();
		ID();
		EX();
		MEM();
		WB();
        //instCnt++;
        //if (instCnt == 50)
            //break;
		//更新中间寄存器
		/*IF_ID=IF_ID_old;
		ID_EX=ID_EX_old;
		EX_MEM=EX_MEM_old;
		MEM_WB=MEM_WB_old;*/

        if (step_flag == 1)
        {
            while (true)
            {
                int flag = 0;
                char input[200] = {0};
                printf("Would you like to continue or check something?\n");
                printf("Usage:  g:run to the end  c:next inst  r:check regs  m: check memory\n");
                scanf("%s", &input);
                switch(input[0])
                {
                case 'g':
                    step_flag = 0;
                    flag = 1;
                    break;
                case 'c':
                    flag = 1;
                    break;
                case 'r':
                    checkReg();
                    break;
                case 'm':
                    unsigned long long addr;
                    while(true)
                    {
                        printf("please input the address:0x");
                        scanf("%llx", &addr);
                        if (addr >= 0 && addr < MAX - 8)
                        {
                            printf("\n8 bytes from %llx:0x%016llx\n", addr, *(unsigned long long*)(memory + addr));
                            break;
                        }
                        else
                            printf("illegal address!\n");
                    }
                    break;
                default:
                    printf("illegal input!\n");
                }
                if (flag == 1)
                    break;
            }
        }


        if(exit_flag==1)
            break;

        reg[0]=0;//一直为零
        printf("\n");

	}
    checkReg();

}


//取指令
void IF()
{
	printf("Current PC:0x%llx\n", PC);
	IF_ID.PC = PC;
	IF_ID.inst = memory.get32(PC);
	//write IF_ID_old
	//IF_ID_old.inst=memory.get32(PC);
	PC=PC+4;
	//IF_ID_old.PC=PC;
}

//译码
void ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;
	int EXTop=0;
	unsigned int EXTsrc=0;
    //ALUSrc:0:reg 1:imm, 2:PC
	char RegDst,ALUop,ALUSrc;
	char Branch,MemRead,MemWrite;
	char RegWrite,MemtoReg;

    string name;

    OP = getbit(inst, 0, 6);
	rd = getbit(inst,7,11);
	func3 = getbit(inst,12,14);
	func7 = getbit(inst, 25, 31);
	func7_s = getbit(inst, 26, 31);
    rs1 = getbit(inst, 15, 19);
    rs2 = getbit(inst, 20, 24);
	//....


	if(OP==OP_R)
	{
		/*if(func3==F3_ADD&&func7==F7_ADD)
		{
            EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
		}
		else
		{

		}*/
		EXTop = 0;
		RegDst = rd;
		ALUSrc = 0;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
//加减乘除：0-3.乘方：4,与或非：5-7,左移右移：8-9
//高位乘：10,比大小：11,取余：12,算术右移：13
		if(func3 == 0)
		{
            switch(func7)
            {
            case 0:
                ALUop = add;
                name = "add";
                break;
            case 1:
                ALUop = mul;
                name = "mul";
                break;
            case 32:
                ALUop = sub;
                name = "sub";
            default:;
            }

		}
		if(func3 == 1)
		{
            switch(func7)
            {
            case 0:
                ALUop = sll;
                name = "sll";
                break;
            case 1:
                ALUop = mulh;
                name = "mulh";
            default:;
            }
		}
		if(func3 == 2)
		{
            ALUop = lt;
            name = "slt";
		}
		if(func3 == 4)
		{
            switch(func7)
            {
            case 0:
                ALUop = xor2;
                name = "xor";
                break;
            case 1:
                ALUop = div;
                name = "div";
            default:;
            }
		}
		if(func3 == 5)
		{
            //logical or arthmetric?
            switch(func7)
            {
            case 0:
                ALUop = srl;
                name = "srl";
                break;
            case 32:
                ALUop = sra;
                name = "sra";
            default:;
            }

		}
		if(func3 == 6)
		{
            switch(func7)
            {
            case 0:
                ALUop = or2;
                name = "or2";
                break;
            case 1:
                ALUop = mod;
                name = "rem";
            default:;
            }
		}
		if(func3 == 7)
		{
            ALUop = and2;
            name = "and";
		}
	}
	else if(OP==OP_I)
    {
        shamt = getbit(inst, 20, 25);
        //需要符号扩展的数
        EXTsrc = getbit(inst, 20, 31);
        EXTop = 12;
		RegDst = rd;
		ALUSrc = 1;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
//加减乘除：0-3.乘方：4,与或非：5-7,左移右移：8-9
//高位乘：10,比大小：11,取余：12, 算术右移：13
        switch(func3)
        {
        case 0:
            ALUop = 0;
            name = "addi";
            break;
        case 1:
            EXTop = 6;
            ALUop = sll;
            name = "slli";
            break;
        case 2:
            ALUop = lt;
            name = "slti";
            break;
        case 4:
            ALUop = xor2;
            name = "xori";
            break;
        case 5:
            EXTop = 6;
            if (func7_s == 0)
            {
                ALUop = srl;
                name = "srli";
                EXTsrc = shamt;
                break;
            }
            else
            {
                ALUop = sra;
                name = "srai";
                EXTsrc = shamt;
                break;
            }
        case 6:
            ALUop = or2;
            name = "ori";
            break;
        case 7:
            ALUop = and2;
            name = "andi";
            break;
        default:;
        }
    }
    else if(OP==OP_SW)
    {
        //需要符号扩展的数
        EXTop = 12;
        EXTsrc = S_IMM(inst);
		RegDst = rd;
		ALUSrc = 1;
		Branch = 0;
		MemRead = 0;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemWrite = 0;
		RegWrite = 0;
		MemtoReg = 0;
        switch(func3)
        {
        case 0:
            MemWrite = 1;
            ALUop = add;
            name = "sb";
            break;
        case 1:
            MemWrite = 2;
            ALUop = add;
            name = "sh";
            break;
        case 2:
            MemWrite = 3;
            ALUop = add;
            name = "sw";
            break;
        case 3:
            MemWrite = 4;
            ALUop = add;
            name = "sd";
            break;
        default:;
        }
    }
    else if(OP==OP_LW)
    {
        EXTop = 12;
        EXTsrc = getbit(inst, 20, 31);
		RegDst = rd;
		ALUSrc = 1;
		Branch = 0;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 1;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 1;
        switch(func3)
        {
        case 0:
            MemRead = 1;
            ALUop = add;
            name = "lb";
            break;
        case 1:
            MemRead = 2;
            ALUop = add;
            name = "lh";
            break;
        case 2:
            MemRead = 3;
            ALUop = add;
            name = "lw";
            break;
        case 3:
            MemRead = 4;
            ALUop = add;
            name = "ld";
            break;
        default:;
        }
    }
    else if(OP==OP_BEQ)
    {
        EXTop = 13;
        EXTsrc = B_IMM(inst);
        RegDst = rd;
		ALUSrc = 0;
		Branch = 1;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 0;
		MemtoReg = 0;
        switch(func3)
        {
        case 0:
            ALUop = equ;
            name = "beq";
            break;
        case 1:
            ALUop = nequ;
            name = "bneq";
            break;
        case 4:
            ALUop = lt;
            name = "blt";
            break;
        case 5:
            ALUop = ge;
            name = "bge";
            break;
        default:;
        }
    }
    else if(OP==OP_JALR)
    {
        EXTop = 12;
        EXTsrc = getbit(inst, 20, 31);
        RegDst = rd;
        //2:4
		ALUSrc = 2;
		//Branch 0:none 1:PC+imm
		Branch = 2;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
		ALUop = add;
		name = "jalr";
    }
    else if (OP == OP_JAL)
    {
        EXTop = 20;
        EXTsrc = J_IMM(inst);
        RegDst = rd;
		ALUSrc = 2;
		//Branch 0:none 1:PC+imm 2:jalr
		Branch = 1;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
		ALUop = add;
		name = "jal";
    }
    else if (OP == OP_IW)
    {
        EXTop = 12;
        EXTsrc = getbit(inst, 20, 31);
        RegDst = rd;
		ALUSrc = 1;
		//Branch 0:none 1:PC+imm 2:jalr
		Branch = 0;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
		ALUop = addiw;
		name = "addiw";
    }
    else if (OP == OP_LUI)
    {
        EXTop = 0;
        EXTsrc = getbit(inst, 12, 31);
        EXTsrc = (EXTsrc << 12);
        RegDst = rd;
		ALUSrc = 1;
		//Branch 0:none 1:PC+imm 2:jalr
		Branch = 0;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
		ALUop = lui;
		name = "lui";
    }
    else if (OP == OP_AUIPC)
    {
        EXTop = 0;
        EXTsrc = getbit(inst, 12, 31);
        EXTsrc = (EXTsrc << 20);
        RegDst = rd;
		ALUSrc = 3;
		//Branch 0:none 1:PC+imm 2:jalr
		Branch = 0;
		//MemRead 0:none 1:byte 2:half 3:word 4:doubleword
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
		ALUop = auipc;
		name = "auipc";
    }
    else if (OP == OP_RW)
    {
        EXTop = 0;
		RegDst = rd;
		ALUSrc = 0;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		RegWrite = 1;
		MemtoReg = 0;
        if (func3 == F3_ADDW)
        {
            switch(func7)
            {
            case 0:
                ALUop = addw;
                name = "addw";
                break;
            case 1:
                ALUop = mulw;
                name = "mulw";
                break;
            case 32:
                ALUop = subw;
                name = "subw";
            default:;
            }
        }
        if (func3 == 4)
        {
            switch(func7)
            {
            case 1:
                ALUop = divw;
                name = "divw";
                break;
            }
        }
        if (func3 == 6)
        {
            switch(func7)
            {
            case 1:
                ALUop = modw;
                name = "remw";
                break;
            }
        }
    }
    cout << "instName:"+ name << endl;
    ID_EX.PC = IF_ID.PC;
    ID_EX.Rd = rd;
    ID_EX.rs1 = rs1;
    ID_EX.rs2 = rs2;
    ID_EX.Imm = ext_signed(EXTsrc, EXTop);
    ID_EX.Ctrl_EX_ALUOp = ALUop;
    ID_EX.Ctrl_EX_ALUSrc = ALUSrc;
    ID_EX.Ctrl_EX_RegDst = RegDst;
    ID_EX.Ctrl_M_Branch = Branch;
    ID_EX.Ctrl_M_MemRead = MemRead;
    ID_EX.Ctrl_M_MemWrite = MemWrite;
    ID_EX.Ctrl_WB_MemtoReg = MemtoReg;
    ID_EX.Ctrl_WB_RegWrite = RegWrite;
	//write ID_EX_old
	/*ID_EX_old.Rd=rd;
	ID_EX_old.Rt=rt;
	ID_EX_old.Imm=ext_signed(EXTsrc,EXTop, len);
	//...

	ID_EX_old.Ctrl_EX_ALUOp=ALUop;*/
	//....

}

//执行
void EX()
{
	//read ID_EX
	long long temp_PC=ID_EX.PC;
	char RegDst=ID_EX.Ctrl_EX_RegDst;
	char ALUOp=ID_EX.Ctrl_EX_ALUOp;
	char ALUSrc = ID_EX.Ctrl_EX_ALUSrc;
	char branch = ID_EX.Ctrl_M_Branch;
	int ex_rs1 = ID_EX.rs1;
	long long imm = (long long)ID_EX.Imm;
	printf("ex_imm:%lld\n", imm);
    //Branch 0:none 1:PC+imm
	//Branch PC calulate
	switch(branch)
	{
    case 1:
        temp_PC += (long long)ID_EX.Imm;
        break;
    case 2:
        printf("%d\n", ID_EX.Imm);
        temp_PC = (long long)reg[ex_rs1] + (long long)ID_EX.Imm;
        temp_PC = temp_PC - (temp_PC % 2);
    default:;
	}
	//...
    //ALUSrc:0:rs1 rs2 1:rs1 imm, 2:PC 4, 3:PC imm
	//choose ALU input number
	long long in1 = 0, in2 = 0;
	int zero = 0;
    switch(ALUSrc)
    {
    case 0:
        in1 = (long long)reg[rs1];
        in2 = (long long)reg[rs2];
        break;
    case 1:
        in1 = (long long)reg[rs1];
        in2 = imm;
        break;
    case 2:
        in1 = ID_EX.PC;
        in2 = 4;
        zero = 1;
        break;
    case 3:
        in1 = ID_EX.PC;
        in2 = imm;
    default:;
    }
	//alu calculate

	REG ALUout;
	long long in1_high = 0;
    long long in1_low = 0;
    long long in2_high = 0;
    long long in2_low = 0;

    long long h1h2 = 0;
    long long h1l2 = 0;
    long long l1h2 = 0;
	//加减乘除：0-3.异或：4,与或非：5-7,左移右移：8-9
    //高位乘：10,比大小：11,取余：12,算术右移：13
    //判等：14 不等：15 大于：16 addiw：18 lui:19 auipc:20
    //addw:21
    switch(ALUOp)
    {
    case add:
        ALUout = in1 + in2;
        /*if (ALUSrc == 2)
            printf("0x%llx\n", ALUout);*/
        break;
    case sub:
        ALUout = in1 - in2;
        break;
    case mul:
        ALUout = in1 * in2;
        break;
    case div:
        ALUout = in1 / in2;
        break;
    case xor2:
        ALUout = in1 ^ in2;
        break;
    case and2:
        ALUout = in1 & in2;
        break;
    case or2:
        ALUout = in1 | in2;
        break;
    case sll:
        ALUout = in1 << in2;
        break;
    case srl:
        ALUout = ((unsigned long long)in1) >> in2;
        break;
    case mulh:
        in1_high = ((in1 & 0xffffffff00000000) >> 32) & 0xffffffff;
        in1_low = in1 & 0xffffffff;
        in2_high = ((in2 & 0xffffffff00000000) >> 32) & 0xffffffff;
        in2_low = in2 & 0xffffffff;

        h1h2 = in1_high * in2_high;
        h1l2 = ((in1_high * in2_low) >> 32) & 0xffffffff;
        l1h2 = ((in2_high * in1_low) >> 32) & 0xffffffff;
        ALUout = h1h2 + h1l2 + l1h2;
        break;
    case lt:
        if (in1 < in2)
            zero = 1;
        break;
    case mod:
        ALUout = in1 % in2;
        break;
    case sra:
        ALUout = ((long long)in1) >> in2;
        break;
    case equ:
        if (in1 == in2)
            zero = 1;
        break;
    case nequ:
        if (in1 != in2)
            zero = 1;
        break;
    case ge:
        if (in1 >= in2)
            zero = 1;
        break;
    case addiw:
        ALUout = (long long)((int)(in1 + (long long)in2));
        break;
    case lui:
        ALUout = (long long)in2;
        break;
    case auipc:
        ALUout = in1 + (long long)in2;
        //printf("auipc:0x%lx\n", ALUout);
        break;
    case addw:
        ALUout = (long long)((int)in1 + (int)in2);
        break;
    case mulw:
        ALUout = (long long)((int)in1 * (int)in2);
        //printf("%lld %lld\n", in1, in2);
        //printf("mulw:0x%lx\n", ALUout);
        break;
    case subw:
        ALUout = (long long)((int)in1 - (int)in2);
        break;
    case divw:
        ALUout = (long long)((int)in1 / (int)in2);
        break;
    case modw:
        ALUout = (long long)((int)in1 % (int)in2);
        break;
    default:;
	}

	//choose reg dst address
	int Reg_Dst;
	if(RegDst)
        Reg_Dst = ID_EX.Rd;
    else
        Reg_Dst = 0;

    if (branch != 0)
    {
        printf("branch zero:%d\n", zero);
    }
    EX_MEM.ALU_out = ALUout;
    EX_MEM.Ctrl_M_Branch = branch;
    EX_MEM.Ctrl_M_MemRead = ID_EX.Ctrl_M_MemRead;
    EX_MEM.Ctrl_M_MemWrite = ID_EX.Ctrl_M_MemWrite;
    EX_MEM.Ctrl_WB_MemtoReg = ID_EX.Ctrl_WB_MemtoReg;
    EX_MEM.Ctrl_WB_RegWrite = ID_EX.Ctrl_WB_RegWrite;
    EX_MEM.PC = temp_PC;
    EX_MEM.RegDst = Reg_Dst;
    EX_MEM.rt = ID_EX.rs2;
    EX_MEM.Zero = zero;
	//write EX_MEM_old
	EX_MEM_old.ALU_out=ALUout;
	EX_MEM_old.PC=temp_PC;
    //.....
}

//访问存储器
void MEM()
{
	//MemRead 0:none 1:byte 2:half 3:word 4:doubleword

	//read EX_MEM
    int read = EX_MEM.Ctrl_M_MemRead;
    int write = EX_MEM.Ctrl_M_MemWrite;
    int zero = EX_MEM.Zero;
    int rs2 = EX_MEM.rt;
    long long ALUout = EX_MEM.ALU_out;
    unsigned long long mem_read;
    unsigned long long temp;
	//complete Branch instruction PC change
    if (zero == 1)
        PC = EX_MEM.PC;
    //Mem r/w 0:none 1:byte 2:half 3:word 4:doubleword
	//read / write memory
    if (read != 0)
    {
        switch(read)
        {
        case 1:
            temp = *memory.getAddress(ALUout);
            break;
        case 2:
            temp = *(short *)(memory + ALUout);
            break;
        case 3:
            temp = *(int *)(memory + ALUout);
            break;
        case 4:
            temp = *(long long *)(memory + ALUout);
            break;
        default:;
        }
        mem_read = temp;
    }
    if (write != 0)
    {
        switch(write)
        {
        case 1:
            temp = (unsigned char)reg[rs2];
            *(unsigned char *)(memory + ALUout) = temp;
            break;
        case 2:
            temp = (unsigned short)reg[rs2];
            *(unsigned short *)(memory + ALUout) = temp;
            break;
        case 3:
            temp = (unsigned int)reg[rs2];
            *(unsigned int *)(memory + ALUout) = temp;
            break;
        case 4:
            temp = (unsigned long long)reg[rs2];
            *(unsigned long long *)(memory + ALUout) = temp;
            break;
        }
    }
    MEM_WB.ALU_out = ALUout;
    MEM_WB.Ctrl_WB_MemtoReg = EX_MEM.Ctrl_WB_MemtoReg;
    MEM_WB.Ctrl_WB_RegWrite = EX_MEM.Ctrl_WB_RegWrite;
    MEM_WB.Mem_read = mem_read;
    MEM_WB.RegDst = EX_MEM.RegDst;
	//write MEM_WB_old
}


//写回
void WB()
{
	//read MEM_WB
    unsigned long long ALUout = MEM_WB.ALU_out;
    char MemtoReg = MEM_WB.Ctrl_WB_MemtoReg;
    char RegWrite = MEM_WB.Ctrl_WB_RegWrite;
    unsigned long long mem_read = MEM_WB.Mem_read;
    int rd = MEM_WB.RegDst;
	//write reg
	if (RegWrite == 1)
	{
        printf("reg write!\n");
        if (MemtoReg == 1)
        {
            reg[rd] = mem_read;
            printf("mem_read:%llx\n", mem_read);
        }
        else
        {
            reg[rd] = ALUout;
            printf("ALUout:0x%llx\n", ALUout);
        }
	}
}

bool open_file(char *filename)
{
	file = fopen(filename,"r");
	if (file != NULL)
        return true;
    else
        return false;
}

void read_elf(char *filename)
{
	if(!open_file(filename))
    {
        printf("can't open elf file!");
        return;
    }
    elf = fopen("elf_info.txt", "w");
	fprintf(elf,"ELF Header:\n");
	read_Elf_header();

	//fprintf(elf,"\n\nSection Headers:\n");
	read_elf_sections();

	//fprintf(elf,"\n\nProgram Headers:\n");
	//load_program();

	fprintf(elf,"\n\nSymbol table:\n");
	read_symtable();

	fclose(elf);
}

void read_Elf_header()
{
	//file should be relocated
	fread(&elf64_hdr,1,sizeof(elf64_hdr),file);

	pnum = (int)elf64_hdr.e_phnum;
	padr = (long long)elf64_hdr.e_phoff;
	psize = (int)elf64_hdr.e_phentsize;

	entry = (long long)elf64_hdr.e_entry;

	snum = (int)elf64_hdr.e_shnum;
	sadr = (long long)elf64_hdr.e_shoff;
	ssize = (int)elf64_hdr.e_shentsize;

    shstrindex = (int)elf64_hdr.e_shstrndx;

    fprintf(elf," magic number:  ");
    for (int i = 0; i < 16; i++)
        fprintf(elf, "%x", elf64_hdr.e_ident[i]);
    fprintf(elf, "\n");

	fprintf(elf," Class:  ELFCLASS64\n");

	fprintf(elf," Data:  little-endian\n");

	fprintf(elf," Version:   \n");

	fprintf(elf," OS/ABI:	 System V ABI\n");

	fprintf(elf," ABI Version:   \n");

	fprintf(elf," Type:   %hx\n", elf64_hdr.e_type);

	fprintf(elf," Machine:   %hx\n", elf64_hdr.e_machine);

	fprintf(elf," Version:  %x\n", elf64_hdr.e_version);

	fprintf(elf," Entry point address:  0x%lx\n", elf64_hdr.e_entry);

	fprintf(elf," Start of program headers: %lu bytes into  file\n", elf64_hdr.e_phoff);

	fprintf(elf," Start of section headers: %lu bytes into  file\n", elf64_hdr.e_shoff);

	fprintf(elf," Flags:  0x%x\n", elf64_hdr.e_flags);

	fprintf(elf," Size of this header: %hu Bytes\n", elf64_hdr.e_ehsize);

	fprintf(elf," Size of program headers: %hu Bytes\n", elf64_hdr.e_phentsize);

	fprintf(elf," Number of program headers: %hu\n", elf64_hdr.e_phnum);

	fprintf(elf," Size of section headers: %hu Bytes\n", elf64_hdr.e_shentsize);

	fprintf(elf," Number of section headers: %hu\n", elf64_hdr.e_shnum);

	fprintf(elf," Section header string table index: %hu\n", elf64_hdr.e_shstrndx);

}

void read_elf_sections()
{
    char sname[20] = {0};
    int ncnt = 0;
	Elf64_Shdr elf64_shdr;
    for(int c=0;c<snum;c++)
	{
		if(c == shstrindex)
        {
            fseek(file, sadr + c * ssize, SEEK_SET);
            fread(&elf64_shdr, 1, sizeof(elf64_shdr), file);
            shstradr = elf64_shdr.sh_offset;
        }

	}
	for(int c=0;c<snum;c++)
	{
        if (fseek(file, sadr + c * ssize, SEEK_SET))
        {
            printf("section error");
            exit(0);
        }
		//file should be relocated
		if (!fread(&elf64_shdr,1,sizeof(elf64_shdr),file))
		{
            printf("section error");
            exit(0);
		}
		unsigned long long adr = shstradr + elf64_shdr.sh_name;
		fseek(file, adr, SEEK_SET);
		fread(&sname[ncnt], 1, 1, file);
		while (sname[ncnt] != 0)
		{
            ncnt++;
            fread(&sname[ncnt], 1, 1, file);
		}
		ncnt = 0;
		#ifdef test
		printf("%s\n", sname);
		#endif
		if (!strcmp(sname, ".strtab"))
            stradr = elf64_shdr.sh_addr + elf64_shdr.sh_offset;
        if (!strcmp(sname, ".text"))
        {
            //endPC = entry + elf64_shdr.sh_size;
            #ifdef test
            printf("0x%lx\n", elf64_shdr.sh_addr);
            printf("%ld\n", elf64_shdr.sh_size);
            #endif // test
        }
        if (!strcmp(sname, ".symtab"))
        {
            symadr = elf64_shdr.sh_offset;
            symnum = elf64_shdr.sh_size / elf64_shdr.sh_entsize;
            symsize = elf64_shdr.sh_entsize;
            #ifdef test2
            printf("%d %d\n", elf64_shdr.sh_size, elf64_shdr.sh_entsize);
            #endif // test2
        }
		/*fprintf(elf," [%3d]\n",c);
		fprintf(elf," Name: ");
		fprintf(elf," Type: ");
		fprintf(elf," Address:  ");
		fprintf(elf," Offest:  \n");
		fprintf(elf," Size:  ");
		fprintf(elf," Entsize:  ");
		fprintf(elf," Flags:   ");
		fprintf(elf," Link:  ");
		fprintf(elf," Info:  ");
		fprintf(elf," Align: \n");*/

 	}
}

int load_program()
{
	Elf64_Phdr elf64_phdr;
	for(int c=0;c<pnum;c++)
	{

        if (fseek(file, padr + c * psize, SEEK_SET))
            return 0;
        if (!fread(&elf64_phdr, 1, psize, file))
            return 0;
        if (elf64_phdr.p_type == PT_LOAD)
        {
            #ifdef test
            printf("0x%lx\n", elf64_phdr.p_offset);
            printf("0x%lx\n", elf64_phdr.p_vaddr);
            #endif
            if (fseek(file, elf64_phdr.p_offset, SEEK_SET))
                return 0;
            if (!fread(memory.getAddress(elf64_phdr.p_vaddr), 1, elf64_phdr.p_memsz, file))
                return 0;
        }
        /*
        //fprintf(elf," [%3d]\n",c);
		fprintf(elf," Type:   ");
		fprintf(elf," Flags:   ");
		fprintf(elf," Offset:   ");
		fprintf(elf," VirtAddr:  ");
		fprintf(elf," PhysAddr:   ");
		fprintf(elf," FileSiz:   ");
		fprintf(elf," MemSiz:   ");
		fprintf(elf," Align:   ");*/
	}
	return 1;
}


void read_symtable()
{
	Elf64_Sym elf64_sym;
	char symname[40] = {0};
	int ncnt = 0;
	for(int c=0;c < symnum;c++)
	{
		fprintf(elf," [%3d]   ",c);
		fseek(file,symadr + symsize * c,SEEK_SET);
		//file should be relocated
		fread(&elf64_sym,1,sizeof(elf64_sym),file);
		unsigned long int adr=stradr + elf64_sym.st_name;
        //printf("%d\n",adr);
        fseek(file,adr,SEEK_SET);
		fread(&symname[ncnt],1,1,file);
		while(symname[ncnt]!=0)
		{
			ncnt++;
			fread(&symname[ncnt],1,1,file);
		}
		ncnt=0;
		fprintf(elf," Name:  %40s  \n",symname);
		if (!strcmp(symname, "main"))
		{
            PC = elf64_sym.st_value;
            printf("main starts at: 0x%lx\n", PC);
            endPC = PC + ((elf64_sym.st_size) / 4) * 4;
            printf("main ends at: 0x%lx\n", endPC);
		}
		if (!strcmp(symname, "__global_pointer$"))
		{
            gp = elf64_sym.st_value;
		}
		unsigned int sttype=(unsigned int)elf64_sym.st_info & 0xF;
		unsigned int stbind=(unsigned int)elf64_sym.st_info >> 4;
		if(stbind==0)
			fprintf(elf," Bind:   LOCAL");
		else if(stbind==1)
			fprintf(elf," Bind:   GLOBAL");
		else if(stbind==2)
			fprintf(elf," Bind:   WEAK");
		else if(stbind==3)
			fprintf(elf," Bind:   NUM");
		fprintf(elf,"\n");
		if(sttype==0)
			fprintf(elf," Type:   NOTYPE");
		else if(sttype==1)
			fprintf(elf," Type:   OBJECT");
		else if(sttype==2)
			fprintf(elf," Type:   FUNC");
		else if(sttype==3)
			fprintf(elf," Type:   SECTION");
		else if(sttype==4)
			fprintf(elf," Type:   FILE");
		else if(sttype==5)
			fprintf(elf," Type:   COMMOM");
		else if(sttype==6)
			fprintf(elf," Type:   TLS");
		else if(sttype==7)
			fprintf(elf," Type:   NUM");

		fprintf(elf," Size:   %d",elf64_sym.st_size);

		fprintf(elf," Value:  %016x\n",elf64_sym.st_value);
	}

}
