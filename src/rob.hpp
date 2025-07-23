#ifndef ROB_H
#define ROB_H

#include "base.hpp"

inline void robWork()
{
    for(int i=0;i<8;i++)
        if(ALU.ok[i].get())
        {
            std::cerr<<"Clock "<<Clk<<" ALU-"<<i<<" broadcast ["<<ALU.robID[i].get()<<","<<ALU.output[i].get()<<"]"<<std::endl;
            ALU.ok[i].set(false);
            ALU.busy[i].set(false);
            const unsigned int pos=ALU.robID[i].get()&(ROBcnt-1);
            rob.val[pos].set(ALU.output[i].get());
            rob.ok[pos].set(true);
        }
    if(rob.hd.get()!=rob.tl.get())
    {
        const unsigned int pos=rob.hd.get()&(ROBcnt-1);
        if(rob.ok[pos].get())
        {
            rob.ok[pos].set(false);
            rob.hd.set(rob.hd.get()+1);
            // virtual broadcast
            rob.clk.set(Clk+1);
            rob.castID.set(rob.hd.get());
            rob.result.set(rob.val[pos].get());
            std::cerr<<"Clock "<<Clk<<" ROB broadcast "<<"["<<rob.hd.get()<<","<<rob.val[pos].get()<<"]"<<std::endl;
            // write register
            const auto dest=rob.dest[pos].get();
            const auto op=rob.op[pos].get().op;
            if(op<=19)
            {
                if(rf.busy[dest].get()&&rf.robID[dest].get()==rob.hd.get())
                {
                    rf.busy[dest].set(false);
                    rf.reg[dest].set(rob.val[pos].get());
                }
            }
        }
    }
}

#endif
