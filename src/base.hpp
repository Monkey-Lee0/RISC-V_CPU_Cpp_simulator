#ifndef CLOCK_H
#define CLOCK_H

#include<vector>
#include<cstring>
#include<unordered_map>

inline unsigned int Clk=0;

class instruction
{
public:
    int op;//name of operation
    int p0;//rd or something else
    int p1;//rs1 or something else
    int p2;//rs2 or something else
    instruction():op(0),p0(0),p1(0),p2(0){}
    instruction(const int op, const int p0, const int p1, const int p2):
        op(op),p0(p0),p1(p1),p2(p2){}
    explicit instruction(const int a):op(a),p0(0),p1(0),p2(0){}
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
    bool flag;
public:
    chronicVariant():oldV(0),newV(0),flag(false)
    {
        copyList.emplace_back(static_cast<void*>(&oldV),static_cast<void*>(&newV),static_cast<void*>(&flag),
            sizeof(T));
    }
    explicit chronicVariant(const T& a)
    {
        oldV=newV=a,flag=false;
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
        if(busy.get()&&Clk-clk.get()>=3&&!ok.get())
        {
            if(typ.get())
                for(int i=0;i<16;i++)
                    output[i].set(mem[addr.get()+i]);
            else
                for(int i=0;i<bytes.get();i++)
                    mem[addr.get()+i]=val[i].get();
            ok.set(true);
        }
    }
    void init(const unsigned int addr,const unsigned int val){mem[addr]=val;}
};
inline Memory Dmem,Imem;

constexpr int RScnt=8;
class ReservationStations
{
public:
    chronicVariant<bool> busy[RScnt];
    chronicVariant<instruction> op[RScnt];
    chronicVariant<int> vj[RScnt];
    chronicVariant<int> vk[RScnt];
    chronicVariant<unsigned int> qj[RScnt];
    chronicVariant<unsigned int> qk[RScnt];
    chronicVariant<unsigned int> dest[RScnt];
    chronicVariant<int> A[RScnt];
};
inline ReservationStations rs;

class RegisterFile
{
public:
    chronicVariant<bool> busy[32];
    chronicVariant<unsigned int> robID[32];
    chronicVariant<unsigned int> reg[32];
};
inline RegisterFile rf;

constexpr int ALUcnt=8;
class ArithmeticLogicUnit
{
public:
    chronicVariant<bool> busy[ALUcnt],ok[ALUcnt];
    chronicVariant<instruction> op[ALUcnt];
    chronicVariant<int> s1[ALUcnt],s2[ALUcnt];
    chronicVariant<unsigned int> robID[ALUcnt];
    chronicVariant<int> output[ALUcnt];
};
inline ArithmeticLogicUnit ALU;

constexpr int ROBcnt=8;
class ReOrderBuffer
{
public:
    chronicVariant<bool> ok[ROBcnt];
    chronicVariant<instruction> op[ROBcnt];
    chronicVariant<int> val[ROBcnt];
    chronicVariant<unsigned int> dest[ROBcnt];
    chronicVariant<unsigned int> hd,tl;
    chronicVariant<unsigned int> clk; // the time of boardcast
    chronicVariant<unsigned int> castID; // the robID of broadcast
    chronicVariant<unsigned int> result; // the result of broadcast
    ReOrderBuffer():hd(1),tl(1){}
};
inline ReOrderBuffer rob;

class LoadStoreBuffer
{
};
inline LoadStoreBuffer lsb;

#endif
