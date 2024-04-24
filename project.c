#include "spimcore.h"

//Written By: Alexander Peacock and Katerina Garkova

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch(ALUControl)
    {
        //add
        case 0: 
            *ALUresult = A + B;
            break;
        //sub
        case 1: 
            *ALUresult = A - B;
            break;
        //set less than
        case 2: 
            if((signed)A < (signed)B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        //set less than unsigned
        case 3: 
            if(A < B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        //AND
        case 4: 
            *ALUresult = A & B;
            break;
        //OR
        case 5: 
            *ALUresult = A | B;
            break;
        //left shift 16
        case 6: 
            *ALUresult = B << 16;
            break;
        //r-type instruction
        case 7: 
            *ALUresult = ~A;
            break;
    }

    //checks if zero
    if(*ALUresult == 0)
    {
        *Zero = 1;
    }
    else 
    {
        *Zero = 0;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if(Mem!=NULL && instruction !=NULL && PC % 4 == 0)
    {
        *instruction = MEM(PC);
        return 0;
    }
    return 1;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    //partition the instructions
    *op = (instruction & 0xfc000000) >> 26; //31-26
    *r1 = (instruction & 0x03e00000) >> 21; //25-21
    *r2 = (instruction & 0x001f0000) >> 16; //20-16
    *r3 = (instruction & 0x0000f800) >> 11; //15-11
    *funct = (instruction & 0x0000003f); //5-0
    *offset = (instruction & 0x0000ffff); //15-0
    *jsec = (instruction & 0x03ffffff); // 25-0
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    //reduces repetition;
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    switch(op)
    {
        //r-type
        case 0x0:
            controls->RegDst = 1;
            controls->ALUOp = 7;
            controls->RegWrite = 1;
            break;
        //lw
        case 0x23:
            controls->RegWrite = 1;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            break;
        //sw
        case 0x2b:
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            break;
        //beq
        case 0x4:
            controls->Branch = 1;
            controls->RegDst = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 2;
            controls->ALUOp = 1;
            break;
        //jump
        case 0x2: 
            controls->Jump = 1;
            controls->RegDst = 2;
            controls->Branch = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 2;
            break;
        //addi
        case 0x8:
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        //lui
        case 0xf:
            controls->RegWrite = 1;
            controls->ALUOp = 6;
            controls->ALUSrc = 1;
            break;
        //stli
        case 0xa:
            controls->ALUOp = 2;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        //stliu
        case 0xb:
            controls->ALUOp = 3;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        
        default: 
            return 1; //halts program      
    }
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //checks for negative number
    if((offset >> 15) == 1)
    {
        *extended_value = offset | 0xFFFF0000;
    }
    else
    {
        *extended_value = offset & 0x0000ffff;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //checks if sign extended
	if (ALUSrc == 1)
		data2 = extended_value;

	//checks if r-type instruction
	if (ALUOp == 7){

	switch (funct)
	{
		//add
		case 0x20:
			ALUOp = 0;
			break;

		//sub
		case 0x22:
			ALUOp = 1;
			break;

		//or
		case 0x25:
			ALUOp = 5;
			break;

		//and
		case 0x24:
			ALUOp = 4;
			break;

		//less than
		case 0x2A:
			ALUOp = 2;
			break;

		//unsigned less than
		case 0x2B: 
			ALUOp = 3;
			break;

		//halt
		default:
			return 1;
	}
    }
	//execute ALU operation
	ALU(data1, data2, ALUOp, ALUresult, Zero);

	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //read from memory
	if (MemRead == 1 && ALUresult % 4 == 0 && ALUresult < 65536)
		*memdata = MEM(ALUresult);
	else if(MemRead == 1)
		return 1;

	//write to memory
	if (MemWrite == 1 && ALUresult % 4 == 0 && ALUresult < 65536)
		MEM(ALUresult) = data2;
	else if (MemWrite == 1)
		return 1;
	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //data comes from memory
    if(RegWrite == 1 && MemtoReg == 1)
    {
        Reg[r2] = memdata;
    }
    //data is coming from a register for r-type
    else if(RegWrite == 1 && MemtoReg == 0 && RegDst == 1)
    {
        Reg[r3] = ALUresult;
    }
    //for i-type instructions
    else if(RegWrite == 1 && MemtoReg == 0 && RegDst == 0)
    {
        Reg[r2] = ALUresult;
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //increments PC by 4 to prepare for instruction
    *PC += 4; 

    //tests zero and branch if need to branch
    if(Branch == 1 && Zero == 1)
    {
        *PC += (extended_value << 2); //shifts left by 2
    }
    else if(Jump == 1)
    {
        //upper four bits of PC combined with jsec shifted left by 2
        *PC =  (jsec << 2) | *PC & 0xF0000000; 
    }

}

