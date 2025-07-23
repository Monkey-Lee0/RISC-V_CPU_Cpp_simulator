// A decoder of machine code, translating them into assembly language

#ifndef DECODER_H
#define DECODER_H

#include "base.hpp"
#include "memory.hpp"

constexpr bool pseudo=true;

inline unsigned int singleHexToDec(const char s)
{
    if(s>='0'&&s<='9')
        return s-'0';
    if(s>='A'&&s<='F')
        return s-'A'+10;
    if(s>='a'&&s<='f')
        return s-'a'+10;
    return -1;
}
inline std::string regName(const int id)
{
    static std::string res[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",
    "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"};
    if(id>=32||id<0)
        return "uk-"+std::to_string(id);
    return res[id];
}
inline std::ostream& operator<<(std::ostream& os,const instruction& other)
{
    const std::string& s=other.op;
    if(pseudo)
    {
        if(s=="beq"&&other.p2==0)
            return os<<"beqz"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="bne"&&other.p2==0)
            return os<<"bnez"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="jal"&&other.p0==0)
            return os<<"j"<<" "<<other.p1;
        if(s=="jal"&&other.p0==1)
            return os<<"jal"<<" "<<other.p1;
        if(s=="jalr"&&other.p0==0&&other.p1==1&&other.p2==0)
            return os<<"ret";
        if(s=="jalr"&&other.p0==0&&other.p2==0)
            return os<<"jr"<<" "<<regName(other.p1);
        if(s=="addi"&&other.p0==0&&other.p1==0&&other.p2==0)
            return os<<"nop";
        if(s=="addi"&&other.p1==0)
            return os<<"li"<<" "<<regName(other.p0)<<" "<<other.p2;
        if(s=="addi"&&other.p2==0)
            return os<<"mv"<<" "<<regName(other.p0)<<" "<<regName(other.p1);
        if(s=="sub"&&other.p1==0)
            return os<<"neg"<<" "<<regName(other.p0)<<" "<<regName(other.p2);
        if(s=="xor"&&other.p2==-1)
            return os<<"not"<<" "<<regName(other.p0)<<" "<<regName(other.p1);
    }
    os<<s;
    if(s=="add"||s=="sub"||s=="and"||s=="or"||s=="xor"||s=="sll"||s=="srl"||s=="sra"||s=="slt"||s=="sltu")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<regName(other.p2);
    else if(s=="addi"||s=="andi"||s=="ori"||s=="xori"||s=="slli"||s=="srli"||s=="srai"||s=="slti"||s=="sltiu")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<other.p2;
    else if(s=="lb"||s=="lbu"||s=="lh"||s=="lhu"||s=="lw"||s=="sb"||s=="sh"||s=="sw")
        os<<" "<<regName(other.p0)<<" "<<other.p2<<"("<<regName(other.p1)<<")";
    else if(s=="beq"||s=="bge"||s=="bgeu"||s=="blt"||s=="bltu"||s=="bne")
        os<<" "<<regName(other.p1)<<" "<<regName(other.p2)<<" "<<other.p0;
    else if(s=="jal")
        os<<" "<<regName(other.p0)<<" "<<other.p1;
    else if(s=="jalr")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<other.p2;
    else if(s=="auipc"||s=="lui")
        os<<" "<<regName(other.p0)<<" "<<other.p1;
    return os;
}

inline instruction instructionDecoder(const unsigned int ins)
{
    const auto opcode=ins&127;
    if(opcode==0b0110011)// type-R
    {
        const int funct3=(ins>>12)&7;
        const int funct7=(ins>>25)&127;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        std::string op;
        if(funct3==0b000)
            op=funct7?"sub":"add";
        else if(funct3==0b111)
            op="and";
        else if(funct3==0b110)
            op="or";
        else if(funct3==0b100)
            op="xor";
        else if(funct3==0b001)
            op="sll";
        else if(funct3==0b101)
            op=funct7?"sra":"srl";
        else if(funct3==0b010)
            op="slt";
        else if(funct3==0b011)
            op="sltu";
        return {op,rd,rs1,rs2};
    }
    if(opcode==0b0010011)// type-I/I*(Arithmetic)
    {
        const int funct3=(ins>>12)&7;
        const int funct7=(ins>>25)&127;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        int imm=(ins>>20)&4095;
        std::string op;
        if(funct3==0b000)
            op="addi",imm=(imm<<20)>>20;
        else if(funct3==0b111)
            op="andi",imm=(imm<<20)>>20;
        else if(funct3==0b110)
            op="ori",imm=(imm<<20)>>20;
        else if(funct3==0b100)
            op="xori",imm=(imm<<20)>>20;
        else if(funct3==0b001)
            op="slli",imm&=31;
        else if(funct3==0b101)
            op=funct7?"srai":"srli",imm&=31;
        else if(funct3==0b010)
            op="slti",imm=(imm<<20)>>20;
        else if(funct3==0b011)
            op="sltiu";
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0000011)// type-I(Memory)
    {
        const int funct3=(ins>>12)&7;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        std::string op;
        if(funct3==0b000)
            op="lb";
        else if(funct3==0b100)
            op="lbu";
        else if(funct3==0b001)
            op="lh";
        else if(funct3==0b101)
            op="lhu";
        else if(funct3==0b010)
            op="lw";
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0100011)// type-S
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>(((ins>>7)&31)|(((ins>>25)&127)<<5))<<20)>>20;
        std::string op;
        if(funct3==0b000)
            op="sb";
        else if(funct3==0b001)
            op="sh";
        else if(funct3==0b010)
            op="sw";
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1100011)// type-B
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>((((ins>>8)&15))|(((ins>>25)&63)<<4)|(((ins>>7)&1)<<10)|(((ins>>31)&1)<<11))<<20)>>19;
        std::string op;
        if(funct3==0b000)
            op="beq";
        else if(funct3==0b101)
            op="bge";
        else if(funct3==0b111)
            op="bgeu";
        else if(funct3==0b100)
            op="blt";
        else if(funct3==0b110)
            op="bltu";
        else if(funct3==0b001)
            op="bne";
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1101111)// type-J
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((((ins>>12)&255)<<11)|(((ins>>20)&1)<<10)|(((ins>>21)&1023))|(((ins>>31)&1)<<19))<<12)>>11;
        const std::string op="jal";
        return {op,rd,imm,0};
    }
    if(opcode==0b1100111)// type-I(Control)
    {
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(((ins>>20)&4095)<<20)>>20;
        const std::string op="jalr";
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0010111)// type-U(auipc)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        const std::string op="auipc";
        return {op,rd,imm,0};
    }
    if(opcode==0b0110111)// type-U(lui)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        const std::string op="lui";
        return {op,rd,imm,0};
    }
    if(opcode==0b1110011)// type-I(Other)
    {
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        std::string op;
        if(!imm)
            op="ebreak";
        else
            op="ecall";
        return {op,0,0,0};
    }
    return {"",0,0,0};
}

inline void decoderWork()
{
    if(IFC.hd.get()+4<=IFC.tl.get())
    {
        PC.set(PC.get()+4);
        IFC.hd.set(IFC.hd.get()+4);
        unsigned int data=0;
        for(int i=3;i>=0;i--)
            data=(data<<8)|IFC.q[(IFC.hd.get()+i)&63].get();
        const auto ins=instructionDecoder(data);
        if(!ins.op.empty())
            std::cerr<<"Clock "<<Clk<<" "<<ins<<std::endl;
    }
}

#endif
