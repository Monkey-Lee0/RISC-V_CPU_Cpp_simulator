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
    static constexpr std::string res[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",
    "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"};
    if(id>=32||id<0)
        return "ukReg-"+std::to_string(id);
    return res[id];
}
inline std::string insName(const int id)
{
    static constexpr std::string res[]={"","add","sub","and","or","xor","sll","srl","sra","slt","sltu",
    "addi","andi","ori","xori","slli","srli","srai","slti","sltiu","lb","lbu","lh","lhu",
    "lw","sb","sh","sw","beq","bge","bgeu","blt","bltu","bne","jal","jalr","auipc",
    "lui","ebreak","ecall"};
    if(id<0||id>=40)
        return "ukIns-"+std::to_string(id);
    return res[id];
}
inline std::ostream& operator<<(std::ostream& os,const instruction& other)
{
    const std::string& s=insName(other.op);
    if(pseudo)
    {
        if(s=="beq"&&other.p2==0)
            return os<<"beqz"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="blt"&&other.p2==0)
            return os<<"bltz"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="bge"&&other.p2==0)
            return os<<"bgez"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="bge"&&other.p1==0)
            return os<<"blez"<<" "<<regName(other.p2)<<" "<<other.p0;
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
    else if(s=="lb"||s=="lbu"||s=="lh"||s=="lhu"||s=="lw")
        os<<" "<<regName(other.p0)<<" "<<other.p2<<"("<<regName(other.p1)<<")";
    else if(s=="sb"||s=="sh"||s=="sw")
        os<<" "<<regName(other.p2)<<" "<<other.p0<<"("<<regName(other.p1)<<")";
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
        int op;
        if(funct3==0b000)
            op=funct7?2:1;
        else if(funct3==0b111)
            op=3;
        else if(funct3==0b110)
            op=4;
        else if(funct3==0b100)
            op=5;
        else if(funct3==0b001)
            op=6;
        else if(funct3==0b101)
            op=funct7?8:7;
        else if(funct3==0b010)
            op=9;
        else if(funct3==0b011)
            op=10;
        return {op,rd,rs1,rs2};
    }
    if(opcode==0b0010011)// type-I/I*(Arithmetic)
    {
        const int funct3=(ins>>12)&7;
        const int funct7=(ins>>25)&127;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        int imm=(ins>>20)&4095;
        int op;
        if(funct3==0b000)
            op=11,imm=(imm<<20)>>20;
        else if(funct3==0b111)
            op=12,imm=(imm<<20)>>20;
        else if(funct3==0b110)
            op=13,imm=(imm<<20)>>20;
        else if(funct3==0b100)
            op=14,imm=(imm<<20)>>20;
        else if(funct3==0b001)
            op=15,imm&=31;
        else if(funct3==0b101)
            op=funct7?17:16,imm&=31;
        else if(funct3==0b010)
            op=18,imm=(imm<<20)>>20;
        else if(funct3==0b011)
            op=19;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0000011)// type-I(Memory)
    {
        const int funct3=(ins>>12)&7;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        int op=0;
        if(funct3==0b000)
            op=20;
        else if(funct3==0b100)
            op=21;
        else if(funct3==0b001)
            op=22;
        else if(funct3==0b101)
            op=23;
        else if(funct3==0b010)
            op=24;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0100011)// type-S
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>(((ins>>7)&31)|(((ins>>25)&127)<<5))<<20)>>20;
        int op;
        if(funct3==0b000)
            op=25;
        else if(funct3==0b001)
            op=26;
        else if(funct3==0b010)
            op=27;
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1100011)// type-B
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>((((ins>>8)&15))|(((ins>>25)&63)<<4)|(((ins>>7)&1)<<10)|(((ins>>31)&1)<<11))<<20)>>19;
        int op;
        if(funct3==0b000)
            op=28;
        else if(funct3==0b101)
            op=29;
        else if(funct3==0b111)
            op=30;
        else if(funct3==0b100)
            op=31;
        else if(funct3==0b110)
            op=32;
        else if(funct3==0b001)
            op=33;
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1101111)// type-J
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((((ins>>12)&255)<<11)|(((ins>>20)&1)<<10)|(((ins>>21)&1023))|(((ins>>31)&1)<<19))<<12)>>11;
        constexpr int op=34;
        return {op,rd,imm,0};
    }
    if(opcode==0b1100111)// type-I(Control)
    {
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(((ins>>20)&4095)<<20)>>20;
        constexpr int op=35;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0010111)// type-U(auipc)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        constexpr int op=36;
        return {op,rd,imm,0};
    }
    if(opcode==0b0110111)// type-U(lui)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        constexpr int op=37;
        return {op,rd,imm,0};
    }
    if(opcode==0b1110011)// type-I(Other)
    {
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        int op;
        if(!imm)
            op=38;
        else
            op=39;
        return {op,0,0,0};
    }
    return {0,0,0,0};
}

inline void decoderWork()
{
    bool ok=false;
    for(auto &i:rs.busy)
        if(!i.get())
            ok=true;
    if(ok&&rob.tl.get()-rob.hd.get()<ROBcnt&&IFC.hd.get()+4<=IFC.tl.get())
    {
        PC.set(PC.get()+4);
        IFC.hd.set(IFC.hd.get()+4);
        unsigned int data=0;
        for(int i=3;i>=0;i--)
            data=(data<<8)|IFC.q[(IFC.hd.get()+i)&63].get();
        const auto ins=instructionDecoder(data);
        if(ins.op<=0||ins.op>=40)
            return ;
        if(ins.op>0&&ins.op<40)
            std::cerr<<"Clock "<<Clk<<" Decoder "<<ins<<std::endl;
        // Insert new item into rob
        rob.tl.set(rob.tl.get()+1);
        const int pos=rob.tl.get()&(ROBcnt-1);
        rob.ok[pos].set(false);
        rob.op[pos].set(ins);
        if(ins.op<=19)
            rob.dest[pos].set(ins.p0);
        // Insert new item into rs
        for(int i=0;i<ROBcnt;i++)
            if(!rs.busy[i].get())
            {
                rs.busy[i].set(true);
                rs.op[i].set(ins);
                if(ins.op<=19)
                {
                    if(rf.busy[ins.p1].get())
                        rs.qj[i].set(rf.robID[ins.p1].get());
                    else
                    {
                        rs.vj[i].set(rf.reg[ins.p1].get());
                        rs.qj[i].set(0);
                    }
                }
                if(ins.op<=10)
                {
                    if(rf.busy[ins.p2].get())
                        rs.qk[i].set(rf.robID[ins.p2].get());
                    else
                    {
                        rs.vk[i].set(rf.reg[ins.p2].get());
                        rs.qk[i].set(0);
                    }
                }
                else if(ins.op<=19)
                    rs.A[i].set(ins.p2);
                rs.dest[i].set(rob.tl.get());
                break;
            }
        // Change RF status
        if(ins.op<=19)
        {
            rf.busy[ins.p0].set(true);
            rf.robID[ins.p0].set(rob.tl.get());
        }
    }
}

#endif
