#include "spimcore.h"


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
            if((int)A < (int)B)
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
            *ALUresult << 16;
            break;
        //r-type instruction
        case 7: 
            *ALUresult = !A;
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
    if(PC % 4 == 0)
    {
        *instruction=Mem[PC >> 2];
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
    *r2 = (instruction & 0x001f0000) >> 16 //20-16
    *r3 = (instruction & 0x0000f800) >> 11 //15-11
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
            controls->RegDst = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 1;
            break;
        //beq
        case 0x4:
            controls->Branch = 1;
            controls->RegDst = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 1;
            controls->ALUOp = 1;
            break;
        //jump
        case 0x2: 
            controls->Jump = 1;
            controls->RegDst = 2;
            controls->Branch = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 2;
            controls->ALUOp = 2;
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
            controls->ALUOp = 3; //TODO: BRO IDK
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
        *extended_value = offset | 0xffff0000;
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
    //decides which ALU operation to send
    switch(ALUOp)
    {
        case 0x0:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;
        
        case 0x1:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;

        case 0x2:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;
        
        case 0x3:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;

        case 0x4:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;

        case 0x5:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;

        case 0x6:
            if(ALUSrc==1)
                ALU(data1, extended_value, ALUControl, ALUresult, Zero);
            else
                ALU(data1, data2, ALUControl, ALUresult, Zero);
            break;
        
        case 0x7:
            switch(funct)
            {
                Case 0x20:
                    ALU(data1, data2, 0, ALUresult, Zero);
                    break;

                Case 0x22:
                    ALU(data1, data2, 1, ALUresult, Zero);
                    break;

                Case 0x2A:
                    ALU(data1, data2, 2, ALUresult, Zero);
                    break;

                Case 0x2B:
                    ALU(data1, data2, 3, ALUresult, Zero);
                    break;

                Case 0x24:
                    ALU(data1, data2, 4, ALUresult, Zero);
                    break;

                Case 0x25:
                    ALU(data1, data2, 5, ALUresult, Zero);
                    break;

                //halt condition occurs
                default:
                    return 1;
            }
            break;

        //halt condition occurs
        default:
            return 1;
    }
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //checks if word-aligned and in range
    if(ALUresult % 4 != 0 || ALUresult < 0 || ALUresult > 0xFFFF)
    {
        return 1;
    }
    //load word
    if(MemRead == 1)
    {
        *memdata = Mem[ALUresult >> 2];
        return 0;
    }
    //store word
    else if(MemWrite == 1)
    {
        Mem[ALUresult >> 2] == data2;
        return 0;
    }

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
    else
    {
        if(RegWrite == 1)
        {
            Reg[r2] = ALUresult;
        }
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
        unsigned upperFour = (*PC & 0xF0000000);
        //upper four bits of PC combined with jsec shifted left by 2
        *PC =  upperFour | (jsec << 2); 
    }

}

