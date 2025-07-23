#ifndef CLOCK_H
#define CLOCK_H

#include<vector>
#include<cstring>
#include<unordered_map>

inline unsigned int Clk=0;

class instruction
{
public:
    std::string op;//name of operation
    int p0;//rd or something else
    int p1;//rs1 or something else
    int p2;//rs2 or something else
    instruction():p0(0),p1(0),p2(0){}
    instruction(const std::string &op, const int p0, const int p1, const int p2):
        op(op),p0(p0),p1(p1),p2(p2){}
};

struct copy
{
    void *dest=nullptr,*src=nullptr,*flag=nullptr;
    size_t size=0;
};

inline std::vector<copy> copyList;
template<class T>
class chronicVariant
{
    T oldV,newV;
    bool flag{};
public:
    chronicVariant():oldV(0),newV(0),flag(false)
    {
        copyList.emplace_back(static_cast<void*>(&oldV),static_cast<void*>(&newV),static_cast<void*>(&flag),
            sizeof(T));
    }
    explicit chronicVariant(const T& a)
    {
        oldV=newV=a;
        copyList.emplace_back(static_cast<void*>(&oldV),static_cast<void*>(&newV),static_cast<void*>(&flag),
            sizeof(T));
    }
    T get(){return oldV;}
    void set(const T& val){newV=val;flag=true;}
};

inline chronicVariant<unsigned int> PC(0);

inline void tick()
{
    Clk++;
    for(auto &[dest,src,flag,size]:copyList)
        if(*static_cast<bool*>(flag))
        {
            memcpy(dest,src,size);
            *static_cast<bool*>(flag)=true;
        }
}

class Memory
{
    std::unordered_map<unsigned int, unsigned int> mem;
public:
    chronicVariant<unsigned int> addr,clk,val[16],bytes,output[16];// bytes->bytes to write
    chronicVariant<bool> typ,busy,ok;// typ->true read;false write
    void work()
    {
        if(busy.get()&&Clk-clk.get()>=3)
        {
            if(typ.get())
                for(int i=0;i<16;i++)
                    output[i].set(mem[addr.get()+i]);
            else
                for(int i=0;i<bytes.get();i++)
                    mem[addr.get()+i]=val[i].get();
            ok.set(true);
            busy.set(false);
        }
    }
    void init(const unsigned int addr,const unsigned int val){mem[addr]=val;}
};

inline Memory Dmem,Imem;

class Unit;

class Wire
{
public:
    Unit* dest=nullptr;
    instruction data;
};

class Unit
{
public:
    instruction I;
    std::vector<Wire> O;
    bool flag=false; // whether the unit is busy.
    virtual ~Unit()=default;
    virtual void work()=0;// I --unit-> O
    virtual void req(const instruction&)=0;// wire--->I
    bool getState()const{return flag;}
};

class UnitGroup
{
public:
    std::vector<Unit*> units;
    void tick()
    {
        for(const auto unit:units)
            unit->work();
        for(const auto unit:units)
            for(const auto& wire:unit->O)
            {
                if(wire.dest==nullptr)
                    continue;
                wire.dest->req(wire.data);
            }
    }
};

#endif
