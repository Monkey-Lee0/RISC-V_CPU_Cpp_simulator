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
            if(id==1||id==11||id==34||id==35||id==36||id==37)
                ALU.output[i].set(ALU.s1[i].get()+ALU.s2[i].get());
            else if(id==2)
                ALU.output[i].set(ALU.s1[i].get()-ALU.s2[i].get());
            else if(id==3||id==12)
                ALU.output[i].set(ALU.s1[i].get()&ALU.s2[i].get());
            else if(id==4||id==13)
                ALU.output[i].set(ALU.s1[i].get()|ALU.s2[i].get());
            else if(id==5||id==14)
                ALU.output[i].set(ALU.s1[i].get()^ALU.s2[i].get());
            else if(id==6||id==15)
                ALU.output[i].set(ALU.s1[i].get()<<ALU.s2[i].get());
            else if(id==7||id==16)
                ALU.output[i].set(static_cast<unsigned int>(ALU.s1[i].get())>>ALU.s2[i].get());
            else if(id==8||id==17)
                ALU.output[i].set(ALU.s1[i].get()>>ALU.s2[i].get());
            else if(id==9||id==18)
                ALU.output[i].set(ALU.s1[i].get()<ALU.s2[i].get());
            else if(id==10||id==19)
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
            if(id==28)
                BPU.output[i].set(BPU.s1[i].get()==BPU.s2[i].get());
            if(id==29)
                BPU.output[i].set(BPU.s1[i].get()>=BPU.s2[i].get());
            if(id==30)
                BPU.output[i].set(static_cast<unsigned int>(BPU.s1[i].get())>=
                    static_cast<unsigned int>(BPU.s2[i].get()));
            if(id==31)
                BPU.output[i].set(BPU.s1[i].get()<BPU.s2[i].get());
            if(id==32)
                BPU.output[i].set(static_cast<unsigned int>(BPU.s1[i].get())<
                static_cast<unsigned int>(BPU.s2[i].get()));
            if(id==33)
                BPU.output[i].set(BPU.s1[i].get()!=BPU.s2[i].get());
        }
}

#endif
