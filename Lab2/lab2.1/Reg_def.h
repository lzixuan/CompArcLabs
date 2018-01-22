typedef unsigned long long REG;

struct IFID{
	unsigned int inst;
	long long PC;
}IF_ID,IF_ID_old;


struct IDEX{
	int Rd;
	long long PC;
	int Imm;
	int rs1, rs2;

	char Ctrl_EX_ALUSrc;
	char Ctrl_EX_ALUOp;
	char Ctrl_EX_RegDst;

	char Ctrl_M_Branch;
	char Ctrl_M_MemWrite;
	char Ctrl_M_MemRead;

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}ID_EX,ID_EX_old;

struct EXMEM{
	long long PC;
	int RegDst;
	REG ALU_out;
	int Zero;
	int rt;

	char Ctrl_M_Branch;
	char Ctrl_M_MemWrite;
	char Ctrl_M_MemRead;

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}EX_MEM,EX_MEM_old;

struct MEMWB{
	unsigned long long Mem_read;
	REG ALU_out;
	int RegDst;

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}MEM_WB,MEM_WB_old;
