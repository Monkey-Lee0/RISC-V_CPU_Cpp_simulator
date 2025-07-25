#ifndef LSB_H
#define LSB_H

#include "base.hpp"

inline void lsbWork()
{
    if(rob.flush.get())
    {
        lsb.tl.set(lsb.hd.get());
        return ;
    }
    if(rob.clk.get()==Clk) // new-completed
        for(int i=lsb.hd.get();i<lsb.tl.get();i++)
        {
            const auto pos=i&(LSBcnt-1);
            if(lsb.qd[pos].get()==rob.castID.get())
            {
                lsb.data[pos].set(rob.result.get());
                lsb.qd[pos].set(0);
            }
        }
    for(int i=0;i<ALUcnt;i++)
        if(AGU.ok[i].get())
        {
            // std::cerr<<"Clock "<<Clk<<" AGU-"<<i<<" broadcast ["<<AGU.ID[i].get()<<","<<AGU.output[i].get()<<"]"<<std::endl;
            AGU.ok[i].set(false);
            AGU.busy[i].set(false);
            lsb.addr[AGU.ID[i].get()&(LSBcnt-1)].set(AGU.output[i].get());
            lsb.qa[AGU.ID[i].get()&(LSBcnt-1)].set(0);
        }
    if(lsb.hd.get()!=lsb.tl.get())
    {
        const unsigned int pos=lsb.hd.get()&(LSBcnt-1);
        const auto op=lsb.op[pos].get().op;
        if(Dmem.ok.get())
        {
            Dmem.ok.set(false);
            Dmem.busy.set(false);
            if(Dmem.typ.get())
            {
                int res=0;
                if(op==20)
                    res=static_cast<int>(Dmem.output[0].get()<<24)>>24;
                if(op==21)
                    res=Dmem.output[0].get();
                if(op==22)
                    res=static_cast<int>(((Dmem.output[1].get()<<8)|Dmem.output[0].get())<<16)>>16;
                if(op==23)
                    res=(Dmem.output[1].get()<<8)|Dmem.output[0].get();
                if(op==24)
                    res=(Dmem.output[3].get()<<24)|(Dmem.output[2].get()<<16)|(Dmem.output[1].get()<<8)|Dmem.output[0].get();
                lsb.output[pos].set(res);
                lsb.ok[pos].set(true);
            }
        }
        if(!lsb.ok[pos].get()&&op<=24&&!Dmem.busy.get()&&!lsb.qa[pos].get())
        {
            Dmem.busy.set(true);
            Dmem.addr.set(lsb.addr[pos].get());
            Dmem.clk.set(Clk);
            Dmem.typ.set(true);
        }
        if(!rob.ok[lsb.robID[pos].get()&(ROBcnt-1)].get()&&op>24&&!lsb.qa[pos].get()&&!lsb.qd[pos].get())
            rob.ok[lsb.robID[pos].get()&(ROBcnt-1)].set(true);
    }
}

#endif