#ifndef ALU_H
#define ALU_H

#include "base.hpp"

inline void aluWork()
{
    if(rob.flush.get())
    {
        for(int i=0;i<ALUcnt;i++)
        {
            ALU.busy[i].set(false);
            ALU.ok[i].set(false);
        }
        return ;
    }
    for(int i=0;i<ALUcnt;i++)
        if(ALU.busy[i].get()&&!ALU.ok[i].get())
        {
            ALU.ok[i].set(true);
            const auto id=ALU.op[i].get().op;
            if(id==OP::ADD||id==OP::ADDI||id==OP::JAL||id==OP::JALR||id==OP::AUIPC||id==OP::LUI)
                ALU.output[i].set(ALU.s1[i].get()+ALU.s2[i].get());
            else if(id==OP::SUB)
                ALU.output[i].set(ALU.s1[i].get()-ALU.s2[i].get());
            else if(id==OP::AND||id==OP::ANDI)
                ALU.output[i].set(ALU.s1[i].get()&ALU.s2[i].get());
            else if(id==OP::OR||id==OP::ORI)
                ALU.output[i].set(ALU.s1[i].get()|ALU.s2[i].get());
            else if(id==OP::XOR||id==OP::XORI)
                ALU.output[i].set(ALU.s1[i].get()^ALU.s2[i].get());
            else if(id==OP::SLL||id==OP::SLLI)
                ALU.output[i].set(ALU.s1[i].get()<<ALU.s2[i].get());
            else if(id==OP::SRL||id==OP::SRLI)
                ALU.output[i].set(static_cast<unsigned int>(ALU.s1[i].get())>>ALU.s2[i].get());
            else if(id==OP::SRA||id==OP::SRAI)
                ALU.output[i].set(ALU.s1[i].get()>>ALU.s2[i].get());
            else if(id==OP::SLT||id==OP::SLTI)
                ALU.output[i].set(ALU.s1[i].get()<ALU.s2[i].get());
            else if(id==OP::SLTU||id==OP::SLTIU)
                ALU.output[i].set(static_cast<unsigned int>(ALU.s1[i].get())<static_cast<unsigned int>(ALU.s2[i].get()));
        }
}

inline void aguWork()
{
    if(rob.flush.get())
    {
        for(int i=0;i<ALUcnt;i++)
        {
            AGU.busy[i].set(false);
            AGU.ok[i].set(false);
        }
        return ;
    }
    for(int i=0;i<ALUcnt;i++)
        if(AGU.busy[i].get()&&!AGU.ok[i].get())
        {
            AGU.ok[i].set(true);
            AGU.output[i].set(AGU.s1[i].get()+AGU.s2[i].get());
        }
}

inline void bpuWork()
{
    if(rob.flush.get())
    {
        for(int i=0;i<ALUcnt;i++)
        {
            BPU.busy[i].set(false);
            BPU.ok[i].set(false);
        }
        return ;
    }
    for(int i=0;i<ALUcnt;i++)
        if(BPU.busy[i].get()&&!BPU.ok[i].get())
        {
            BPU.ok[i].set(true);
            const auto id=BPU.op[i].get().op;
            if(id==OP::BEQ)
                BPU.output[i].set(BPU.s1[i].get()==BPU.s2[i].get());
            if(id==OP::BGE)
                BPU.output[i].set(BPU.s1[i].get()>=BPU.s2[i].get());
            if(id==OP::BGEU)
                BPU.output[i].set(static_cast<unsigned int>(BPU.s1[i].get())>=
                    static_cast<unsigned int>(BPU.s2[i].get()));
            if(id==OP::BLT)
                BPU.output[i].set(BPU.s1[i].get()<BPU.s2[i].get());
            if(id==OP::BLTU)
                BPU.output[i].set(static_cast<unsigned int>(BPU.s1[i].get())<
                static_cast<unsigned int>(BPU.s2[i].get()));
            if(id==OP::BNE)
                BPU.output[i].set(BPU.s1[i].get()!=BPU.s2[i].get());
        }
}

#endif
