#ifndef ALU_H
#define ALU_H

#include "base.hpp"

inline void aluWork()
{
    for(int i=0;i<ALUcnt;i++)
        if(ALU.busy[i].get()&&!ALU.ok[i].get())
        {
            ALU.ok[i].set(true);
            const auto id=ALU.op[i].get().op;
            if(id==1||id==11)
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

#endif
