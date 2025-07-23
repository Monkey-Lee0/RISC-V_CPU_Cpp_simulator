#ifndef MEMORY_H
#define MEMORY_H

#include "base.hpp"

class IFetchCache
{
public:
    chronicVariant<unsigned int> q[64];
    chronicVariant<unsigned int> hd,tl;
    void work()
    {
        if(tl.get()-hd.get()<=48&&Imem.ok.get())
        {
            Imem.ok.set(false);
            Imem.busy.set(false);
            for(int i=0;i<16;i++)
                q[(tl.get()+i)&63].set(Imem.output[i].get());
            tl.set(tl.get()+16);
        }
        else if(tl.get()-hd.get()<=48&&!Imem.busy.get())
        {
            Imem.busy.set(true);
            Imem.addr.set(PC.get()-hd.get()+tl.get());
            Imem.clk.set(Clk);
            Imem.typ.set(true);
        }
    }
};

inline IFetchCache IFC;

#endif
