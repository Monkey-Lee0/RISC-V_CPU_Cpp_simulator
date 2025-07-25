// A decoder of machine code, translating them into assembly language

#ifndef DECODER_H
#define DECODER_H

#include "base.hpp"
#include "memory.hpp"

inline void decoderWork()
{
    if(rob.flush.get())
    {
        PC.set(rob.rightPC.get()+4);
        return ;
    }
    bool ok=false;
    for(auto &i:rs.busy)
        if(!i.get())
            ok=true;
    if(ok&&rob.tl.get()-rob.hd.get()<ROBcnt&&IFC.hd.get()+4<=IFC.tl.get())
    {
        IFC.hd.set(IFC.hd.get()+4);
        unsigned int data=0;
        for(int i=3;i>=0;i--)
            data=(data<<8)|IFC.q[(IFC.hd.get()+i)&63].get();
        const auto ins=instructionDecoder(data);
        if(ins.op<=0||ins.op>=40)
            return ;
        // if(ins.op>0&&ins.op<40)
            // std::cerr<<"Clock "<<Clk<<" Decoder "<<ins<<" "<<rob.tl.get()<<std::endl;
        // update PC
        if(ins.op<=27)
            PC.set(PC.get()+4);
        else if(ins.op<=33)
            PC.set(PC.get()+ins.p0);
        else if(ins.op==34)
            PC.set(PC.get()+ins.p1);
        else
            PC.set(PC.get()+4);
        // Insert new item into rob
        rob.tl.set(rob.tl.get()+1);
        const int pos=rob.tl.get()&(ROBcnt-1);
        rob.ok[pos].set(false);
        rob.op[pos].set(ins);
        rob.curPC[pos].set(PC.get());
        if(ins.op<=24||(ins.op>=34&&ins.op<=37))
            rob.dest[pos].set(ins.p0);
        // Insert new item into lsb
        if(ins.op>=20&&ins.op<=27)
        {
            const unsigned int pos=lsb.tl.get()&(LSBcnt-1);
            lsb.tl.set(lsb.tl.get()+1);
            lsb.ok[pos].set(false);
            lsb.op[pos].set(ins);
            lsb.qa[pos].set(rob.tl.get());
            lsb.robID[pos].set(rob.tl.get());
            if(ins.op>=25)
            {
                if(rf.busy[ins.p2].get())
                    lsb.qd[pos].set(rf.robID[ins.p2].get());
                else
                {
                    lsb.data[pos].set(rf.reg[ins.p2].get());
                    lsb.qd[pos].set(0);
                }
            }
            else
                lsb.qd[pos].set(0);
        }
        // Insert new item into rs
        if(ins.op<=37)
            for(int i=0;i<ROBcnt;i++)
                if(!rs.busy[i].get())
                {
                    rs.busy[i].set(true);
                    rs.op[i].set(ins);
                    if(ins.op==34)
                    {
                        rs.vj[i].set(PC.get()),rs.vk[i].set(4);
                        rs.qj[i].set(0),rs.qk[i].set(0);
                    }
                    if(ins.op==35)
                    {
                        rs.vk[i].set(PC.get()+4),rs.qk[i].set(0);
                        if(rf.busy[ins.p1].get())
                            rs.qj[i].set(rf.robID[ins.p1].get());
                        else
                        {
                            rs.vj[i].set(rf.reg[ins.p1].get());
                            rs.qj[i].set(0);
                        }
                    }
                    if(ins.op==36)
                    {
                        rs.vj[i].set(PC.get()),rs.vk[i].set(ins.p1<<12);
                        rs.qj[i].set(0),rs.qk[i].set(0);
                    }
                    if(ins.op==37)
                    {
                        rs.vj[i].set(0),rs.vk[i].set(ins.p1<<12);
                        rs.qj[i].set(0),rs.qk[i].set(0);
                    }
                    if(ins.op<=33)
                    {
                        if(rf.busy[ins.p1].get())
                            rs.qj[i].set(rf.robID[ins.p1].get());
                        else
                        {
                            rs.vj[i].set(rf.reg[ins.p1].get());
                            rs.qj[i].set(0);
                        }
                    }
                    if(ins.op<=10||(ins.op>=28&&ins.op<=33))
                    {
                        if(rf.busy[ins.p2].get())
                            rs.qk[i].set(rf.robID[ins.p2].get());
                        else
                        {
                            rs.vk[i].set(rf.reg[ins.p2].get());
                            rs.qk[i].set(0);
                        }
                    }
                    else if(ins.op>=11&&ins.op<=27)
                    {
                        if(ins.op<=24)
                            rs.A[i].set(ins.p2);
                        else
                            rs.A[i].set(ins.p0);
                        rs.qk[i].set(0);
                    }
                    if(ins.op<=19||(ins.op>=28&&ins.op<=37))
                        rs.dest[i].set(rob.tl.get());
                    else
                        rs.dest[i].set(lsb.tl.get());
                    break;
                }
        // Change RF status
        if(ins.op<=24||(ins.op>=34&&ins.op<=37))
        {
            rf.busy[ins.p0].set(true);
            rf.robID[ins.p0].set(rob.tl.get());
        }
    }
}

#endif
