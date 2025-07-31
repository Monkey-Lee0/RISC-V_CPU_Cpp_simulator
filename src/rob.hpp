#ifndef ROB_H
#define ROB_H

#include "base.hpp"

inline void robWork()
{
    if(rob.flush.get())
    {
        rob.flush.set(false);
        rob.tl.set(rob.hd.get());
        // clear rf
        for(auto &i:rf.busy)
            i.set(false);
        return ;
    }
    // listen ALU
    for(int i=0;i<8;i++)
        if(ALU.ok[i].get())
        {
            // std::cerr<<"Clock "<<Clk<<" ALU-"<<i<<" broadcast ["<<ALU.ID[i].get()<<","<<ALU.output[i].get()<<"] "<<std::endl;
            ALU.ok[i].set(false);
            ALU.busy[i].set(false);
            const unsigned int pos=ALU.ID[i].get()&(ROBcnt-1);
            rob.val[pos].set(ALU.output[i].get());
            rob.ok[pos].set(true);
        }
    // listen BPU
    for(int i=0;i<8;i++)
        if(BPU.ok[i].get())
        {
            // std::cerr<<"Clock "<<Clk<<" BPU-"<<i<<" broadcast ["<<BPU.ID[i].get()<<","<<BPU.output[i].get()<<"]"<<std::endl;
            BPU.ok[i].set(false);
            BPU.busy[i].set(false);
            const unsigned int pos=BPU.ID[i].get()&(ROBcnt-1);
            rob.val[pos].set(BPU.output[i].get());
            rob.ok[pos].set(true);
        }
    // listen lsb
    if(lsb.hd.get()!=lsb.tl.get())
    {
        const unsigned int pos=lsb.hd.get()&(LSBcnt-1);
        const auto op=lsb.op[pos].get().op;
        if(op<=OP::LW&&lsb.ok[pos].get())
        {
            rob.val[lsb.robID[pos].get()&(ROBcnt-1)].set(lsb.output[pos].get());
            // std::cerr<<"Clock "<<Clk<<" LSB broadcast ["<<lsb.robID[pos].get()<<","<<lsb.output[pos].get()<<"]"<<std::endl;
            rob.ok[lsb.robID[pos].get()&(ROBcnt-1)].set(true);
            lsb.hd.set(lsb.hd.get()+1);
        }
    }
    // try to commit
    if(rob.hd.get()!=rob.tl.get())
    {
        const unsigned int pos=rob.hd.get()&(ROBcnt-1);
        const auto op=rob.op[pos].get().op;
        if(op>=OP::SB&&op<=OP::SW&&Dmem.busy.get()) // cannot commit if Dmem is working.
            return ;
        if(rob.ok[pos].get())
        {
            rob.ok[pos].set(false);
            rob.hd.set(rob.hd.get()+1);
            // virtual broadcast
            rob.clk.set(Clk+1);
            rob.castID.set(rob.hd.get());
            rob.result.set(rob.val[pos].get());
            // std::cerr<<"Clock "<<Clk<<" ROB broadcast "<<"["<<rob.hd.get()<<","<<rob.val[pos].get()<<"]"<<" "<<rob.curPC[pos].get()<<" "<<rob.op[pos].get()<<std::endl;
            // check branch
            if(op>=OP::BEQ&&op<=OP::BNE&&!rob.val[pos].get())
            {
                // flush
                // std::cerr<<"Clock "<<Clk<<" Branch error!"<<std::endl;
                rob.flush.set(true);
                rob.rightPC.set(rob.curPC[pos].get());
                return ;
            }
            // check exit
            if(rob.op[pos].get().op==OP::ADDI&&rob.op[pos].get().p0==10&&
                rob.op[pos].get().p1==0&&rob.op[pos].get().p2==255)
            {
                // std::cout<<"return value "<<(rf.reg[10].get()&255)<<std::endl;
                std::cout<<(rf.reg[10].get()&255)<<std::endl;
                std::cerr<<"Total: "<<Clk<<std::endl;
                exit(0);
            }
            // write register
            const auto dest=rob.dest[pos].get();
            if(op<=OP::LW||(op>=OP::JAL&&op<=OP::LUI))
            {
                if(rf.busy[dest].get())
                {
                    if(rf.robID[dest].get()==rob.hd.get())
                        rf.busy[dest].set(false);
                    rf.reg[dest].set(rob.val[pos].get());
                }
            }
            // write memory
            if(op>=OP::SB&&op<=OP::SW)
            {
                const auto LSBpos=lsb.hd.get()&(LSBcnt-1);
                const auto data=lsb.data[LSBpos].get();
                lsb.hd.set(lsb.hd.get()+1);
                Dmem.busy.set(true);
                Dmem.typ.set(false);
                Dmem.clk.set(Clk);
                Dmem.addr.set(lsb.addr[LSBpos].get());
                if(op==OP::SB)
                {
                    Dmem.bytes.set(1);
                    Dmem.val[0].set(data&255);
                }
                if(op==OP::SH)
                {
                    Dmem.bytes.set(2);
                    Dmem.val[0].set(data&255);
                    Dmem.val[1].set((data>>8)&255);
                }
                if(op==OP::SW)
                {
                    Dmem.bytes.set(4);
                    Dmem.val[0].set(data&255);
                    Dmem.val[1].set((data>>8)&255);
                    Dmem.val[2].set((data>>16)&255);
                    Dmem.val[3].set((data>>24)&255);
                }
            }
        }
    }
}

#endif
