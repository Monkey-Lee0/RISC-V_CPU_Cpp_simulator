#ifndef RS_H
#define RS_H

#include "base.hpp"

inline void rsWork()
{
    if(rob.flush.get())
    {
        for(auto &i:rs.busy)
            i.set(false);
        return ;
    }
    if(rob.clk.get()==Clk) // new-completed
        for(int i=0;i<RScnt;i++)
            if(rs.busy[i].get())
            {
                if(rs.qj[i].get()==rob.castID.get())
                {
                    rs.vj[i].set(rob.result.get());
                    rs.qj[i].set(0);
                }
                if(rs.qk[i].get()==rob.castID.get())
                {
                    rs.vk[i].set(rob.result.get());
                    rs.qk[i].set(0);
                }
            }
    for(int i=0;i<RScnt;i++)
        if(rs.busy[i].get()&&!rs.qj[i].get()&&!rs.qk[i].get())
        {
            if(rs.op[i].get().op<=OP::SLTIU||rs.op[i].get().op==OP::JAL||
                rs.op[i].get().op==OP::AUIPC||rs.op[i].get().op==OP::LUI)
                for(int j=0;j<ALUcnt;j++)
                {
                    if(!ALU.busy[j].get())
                    {
                        rs.busy[i].set(false);
                        ALU.busy[j].set(true);
                        ALU.op[j].set(rs.op[i].get());
                        ALU.s1[j].set(rs.vj[i].get());
                        if(rs.op[i].get().op<=OP::SLTU||rs.op[i].get().op==OP::JAL||
                            rs.op[i].get().op==OP::AUIPC||rs.op[i].get().op==OP::LUI)
                            ALU.s2[j].set(rs.vk[i].get());
                        else if(rs.op[i].get().op<=OP::SLTIU)
                            ALU.s2[j].set(rs.A[i].get());
                        ALU.ID[j].set(rs.dest[i].get());
                        break;
                    }
                }
            else if(rs.op[i].get().op==OP::JALR)
                for(int j=0;j<ALUcnt;j++)
                {
                    if(!ALU.busy[j].get())
                    {
                        rs.busy[i].set(false);
                        IFC.bubble.set(false);
                        unsigned int newPC=rs.vj[i].get()+rs.op[i].get().p2;
                        if(newPC&1)
                            newPC^=1;
                        IFC.predictedPC.set(newPC);
                        PC.set(newPC);
                        ALU.busy[j].set(true);
                        ALU.op[j].set(rs.op[i].get());
                        ALU.s1[j].set(rs.vk[i].get());
                        ALU.s2[j].set(0);
                        ALU.ID[j].set(rs.dest[i].get());
                        break;
                    }
                }
            else if(rs.op[i].get().op<=OP::SW)
                for(int j=0;j<ALUcnt;j++)
                {
                    if(!AGU.busy[j].get())
                    {
                        rs.busy[i].set(false);
                        AGU.busy[j].set(true);
                        AGU.op[j].set(rs.op[i].get());
                        AGU.s1[j].set(rs.vj[i].get());
                        AGU.s2[j].set(rs.A[i].get());
                        AGU.ID[j].set(rs.dest[i].get());
                        break;
                    }
                }
            else if(rs.op[i].get().op<=OP::BNE)
                for(int j=0;j<ALUcnt;j++)
                    if(!BPU.busy[j].get())
                    {
                        rs.busy[i].set(false);
                        BPU.busy[j].set(true);
                        BPU.op[j].set(rs.op[i].get());
                        BPU.s1[j].set(rs.vj[i].get());
                        BPU.s2[j].set(rs.vk[i].get());
                        BPU.ID[j].set(rs.dest[i].get());
                        break;
                    }
            break;
        }
}

#endif
