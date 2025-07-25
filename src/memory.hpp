#ifndef MEMORY_H
#define MEMORY_H

#include "base.hpp"

class IFetchCache
{
public:
    chronicVariant<unsigned int> q[64];
    chronicVariant<unsigned int> hd,tl;
    chronicVariant<unsigned int> predictedPC; // the PC of tl
    void work()
    {
        if(rob.flush.get())
        {
            predictedPC.set(rob.rightPC.get()+4);
            tl.set(hd.get());
            return ;
        }
        if(tl.get()-hd.get()<=48&&Imem.ok.get())
        {
            Imem.ok.set(false);
            Imem.busy.set(false);
            int nPC=predictedPC.get(),ntl=tl.get();
            for(int i=0;i<16;i++)
            {
                q[(tl.get()+i)&63].set(Imem.output[i].get());
                ntl++;
                if(i%4==3)
                {
                    const auto ins=instructionDecoder(Imem.output[i-3].get()|(Imem.output[i-2].get()<<8)|
                        (Imem.output[i-1].get()<<16)|(Imem.output[i].get()<<24));
                    if(ins.op>=28&&ins.op<=33)
                    {
                        nPC+=ins.p0;
                        break;
                    }
                    if(ins.op==34)
                    {
                        nPC+=ins.p1;
                        break;
                    }
                }
                if(i%4==3)
                    nPC+=4;
            }
            tl.set(ntl);
            predictedPC.set(nPC);
        }
        else if(tl.get()-hd.get()<=48&&!Imem.busy.get())
        {
            Imem.busy.set(true);
            Imem.addr.set(predictedPC.get());
            Imem.clk.set(Clk);
            Imem.typ.set(true);
        }
    }
};

inline IFetchCache IFC;

#endif
