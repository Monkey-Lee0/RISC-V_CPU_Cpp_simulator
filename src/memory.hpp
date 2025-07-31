#ifndef MEMORY_H
#define MEMORY_H

#include "base.hpp"

inline void IFCwork()
{
    if(rob.flush.get())
    {
        IFC.predictedPC.set(rob.rightPC.get()+4);
        IFC.tl.set(IFC.hd.get());
        IFC.bubble.set(false);
        return ;
    }
    if(IFC.bubble.get())
        return ;
    if(IFC.tl.get()-IFC.hd.get()<=48&&Imem.ok.get())
    {
        Imem.ok.set(false);
        Imem.busy.set(false);
        int nPC=IFC.predictedPC.get(),ntl=IFC.tl.get();
        for(int i=0;i<16;i++)
        {
            IFC.q[(IFC.tl.get()+i)&63].set(Imem.output[i].get());
            ntl++;
            if(i%4==3)
            {
                const auto ins=instructionDecoder(Imem.output[i-3].get()|(Imem.output[i-2].get()<<8)|
                    (Imem.output[i-1].get()<<16)|(Imem.output[i].get()<<24));
                if(ins.op>=OP::BEQ&&ins.op<=OP::BNE)
                {
                    nPC+=ins.p0;
                    break;
                }
                if(ins.op==OP::JAL)
                {
                    nPC+=ins.p1;
                    break;
                }
                if(ins.op==OP::JALR)
                {
                    IFC.bubble.set(true);
                    break;
                }
            }
            if(i%4==3)
                nPC+=4;
        }
        IFC.tl.set(ntl);
        IFC.predictedPC.set(nPC);
    }
    else if(IFC.tl.get()-IFC.hd.get()<=48&&!Imem.busy.get())
    {
        Imem.busy.set(true);
        Imem.addr.set(IFC.predictedPC.get());
        Imem.clk.set(Clk);
        Imem.typ.set(true);
    }
}

#endif
