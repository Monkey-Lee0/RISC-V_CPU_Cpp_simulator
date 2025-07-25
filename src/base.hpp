#ifndef CLOCK_H
#define CLOCK_H

#include<vector>
#include<cstring>
#include<unordered_map>

inline unsigned int Clk=0;
inline constexpr bool pseudo=true;

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
inline unsigned int singleHexToDec(const char s)
{
    if(s>='0'&&s<='9')
        return s-'0';
    if(s>='A'&&s<='F')
        return s-'A'+10;
    if(s>='a'&&s<='f')
        return s-'a'+10;
    return -1;
}
inline std::string regName(const int id)
{
    static constexpr std::string res[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",
    "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"};
    if(id>=32||id<0)
        return "ukReg-"+std::to_string(id);
    return res[id];
}
inline std::string insName(const int id)
{
    static constexpr std::string res[]={"","add","sub","and","or","xor","sll","srl","sra","slt","sltu",
    "addi","andi","ori","xori","slli","srli","srai","slti","sltiu","lb","lbu","lh","lhu",
    "lw","sb","sh","sw","beq","bge","bgeu","blt","bltu","bne","jal","jalr","auipc",
    "lui","ebreak","ecall"};
    if(id<0||id>=40)
        return "ukIns-"+std::to_string(id);
    return res[id];
}
inline std::ostream& operator<<(std::ostream& os,const instruction& other)
{
    const std::string& s=insName(other.op);
    if(pseudo)
    {
        if(s=="beq"&&other.p2==0)
            return os<<"beqz"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="blt"&&other.p2==0)
            return os<<"bltz"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="bge"&&other.p2==0)
            return os<<"bgez"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="bge"&&other.p1==0)
            return os<<"blez"<<" "<<regName(other.p2)<<" "<<other.p0;
        if(s=="bne"&&other.p2==0)
            return os<<"bnez"<<" "<<regName(other.p1)<<" "<<other.p0;
        if(s=="jal"&&other.p0==0)
            return os<<"j"<<" "<<other.p1;
        if(s=="jal"&&other.p0==1)
            return os<<"jal"<<" "<<other.p1;
        if(s=="jalr"&&other.p0==0&&other.p1==1&&other.p2==0)
            return os<<"ret";
        if(s=="jalr"&&other.p0==0&&other.p2==0)
            return os<<"jr"<<" "<<regName(other.p1);
        if(s=="addi"&&other.p0==0&&other.p1==0&&other.p2==0)
            return os<<"nop";
        if(s=="addi"&&other.p1==0)
            return os<<"li"<<" "<<regName(other.p0)<<" "<<other.p2;
        if(s=="addi"&&other.p2==0)
            return os<<"mv"<<" "<<regName(other.p0)<<" "<<regName(other.p1);
        if(s=="sub"&&other.p1==0)
            return os<<"neg"<<" "<<regName(other.p0)<<" "<<regName(other.p2);
        if(s=="xor"&&other.p2==-1)
            return os<<"not"<<" "<<regName(other.p0)<<" "<<regName(other.p1);
    }
    os<<s;
    if(s=="add"||s=="sub"||s=="and"||s=="or"||s=="xor"||s=="sll"||s=="srl"||s=="sra"||s=="slt"||s=="sltu")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<regName(other.p2);
    else if(s=="addi"||s=="andi"||s=="ori"||s=="xori"||s=="slli"||s=="srli"||s=="srai"||s=="slti"||s=="sltiu")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<other.p2;
    else if(s=="lb"||s=="lbu"||s=="lh"||s=="lhu"||s=="lw")
        os<<" "<<regName(other.p0)<<" "<<other.p2<<"("<<regName(other.p1)<<")";
    else if(s=="sb"||s=="sh"||s=="sw")
        os<<" "<<regName(other.p2)<<" "<<other.p0<<"("<<regName(other.p1)<<")";
    else if(s=="beq"||s=="bge"||s=="bgeu"||s=="blt"||s=="bltu"||s=="bne")
        os<<" "<<regName(other.p1)<<" "<<regName(other.p2)<<" "<<other.p0;
    else if(s=="jal")
        os<<" "<<regName(other.p0)<<" "<<other.p1;
    else if(s=="jalr")
        os<<" "<<regName(other.p0)<<" "<<regName(other.p1)<<" "<<other.p2;
    else if(s=="auipc"||s=="lui")
        os<<" "<<regName(other.p0)<<" "<<other.p1;
    return os;
}

inline instruction instructionDecoder(const unsigned int ins)
{
    const auto opcode=ins&127;
    if(opcode==0b0110011)// type-R
    {
        const int funct3=(ins>>12)&7;
        const int funct7=(ins>>25)&127;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        int op;
        if(funct3==0b000)
            op=funct7?2:1;
        else if(funct3==0b111)
            op=3;
        else if(funct3==0b110)
            op=4;
        else if(funct3==0b100)
            op=5;
        else if(funct3==0b001)
            op=6;
        else if(funct3==0b101)
            op=funct7?8:7;
        else if(funct3==0b010)
            op=9;
        else if(funct3==0b011)
            op=10;
        return {op,rd,rs1,rs2};
    }
    if(opcode==0b0010011)// type-I/I*(Arithmetic)
    {
        const int funct3=(ins>>12)&7;
        const int funct7=(ins>>25)&127;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        int imm=(ins>>20)&4095;
        int op;
        if(funct3==0b000)
            op=11,imm=(imm<<20)>>20;
        else if(funct3==0b111)
            op=12,imm=(imm<<20)>>20;
        else if(funct3==0b110)
            op=13,imm=(imm<<20)>>20;
        else if(funct3==0b100)
            op=14,imm=(imm<<20)>>20;
        else if(funct3==0b001)
            op=15,imm&=31;
        else if(funct3==0b101)
            op=funct7?17:16,imm&=31;
        else if(funct3==0b010)
            op=18,imm=(imm<<20)>>20;
        else if(funct3==0b011)
            op=19;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0000011)// type-I(Memory)
    {
        const int funct3=(ins>>12)&7;
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        int op=0;
        if(funct3==0b000)
            op=20;
        else if(funct3==0b100)
            op=21;
        else if(funct3==0b001)
            op=22;
        else if(funct3==0b101)
            op=23;
        else if(funct3==0b010)
            op=24;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0100011)// type-S
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>(((ins>>7)&31)|(((ins>>25)&127)<<5))<<20)>>20;
        int op;
        if(funct3==0b000)
            op=25;
        else if(funct3==0b001)
            op=26;
        else if(funct3==0b010)
            op=27;
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1100011)// type-B
    {
        const int funct3=(ins>>12)&7;
        const int rs1=(ins>>15)&31;
        const int rs2=(ins>>20)&31;
        const int imm=(static_cast<int>((((ins>>8)&15))|(((ins>>25)&63)<<4)|(((ins>>7)&1)<<10)|(((ins>>31)&1)<<11))<<20)>>19;
        int op;
        if(funct3==0b000)
            op=28;
        else if(funct3==0b101)
            op=29;
        else if(funct3==0b111)
            op=30;
        else if(funct3==0b100)
            op=31;
        else if(funct3==0b110)
            op=32;
        else if(funct3==0b001)
            op=33;
        return {op,imm,rs1,rs2};
    }
    if(opcode==0b1101111)// type-J
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((((ins>>12)&255)<<11)|(((ins>>20)&1)<<10)|(((ins>>21)&1023))|(((ins>>31)&1)<<19))<<12)>>11;
        constexpr int op=34;
        return {op,rd,imm,0};
    }
    if(opcode==0b1100111)// type-I(Control)
    {
        const int rd=(ins>>7)&31;
        const int rs1=(ins>>15)&31;
        const int imm=(((ins>>20)&4095)<<20)>>20;
        constexpr int op=35;
        return {op,rd,rs1,imm};
    }
    if(opcode==0b0010111)// type-U(auipc)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        constexpr int op=36;
        return {op,rd,imm,0};
    }
    if(opcode==0b0110111)// type-U(lui)
    {
        const int rd=(ins>>7)&31;
        const int imm=(static_cast<int>((ins>>12)&1048575)<<12)>>12;
        constexpr int op=37;
        return {op,rd,imm,0};
    }
    if(opcode==0b1110011)// type-I(Other)
    {
        const int imm=(static_cast<int>((ins>>20)&4095)<<20)>>20;
        int op;
        if(!imm)
            op=38;
        else
            op=39;
        return {op,0,0,0};
    }
    return {0,0,0,0};
}

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

constexpr int ROBcnt=8;
class ReOrderBuffer
{
public:
    chronicVariant<bool> ok[ROBcnt];
    chronicVariant<instruction> op[ROBcnt];
    chronicVariant<unsigned int> curPC[ROBcnt]; // the PC of the current instruction
    chronicVariant<int> val[ROBcnt];
    chronicVariant<unsigned int> dest[ROBcnt];
    chronicVariant<unsigned int> clk; // the time of boardcast
    chronicVariant<unsigned int> castID; // the robID of broadcast
    chronicVariant<unsigned int> result; // the result of broadcast
    chronicVariant<bool> flush;
    chronicVariant<unsigned int> rightPC;
    chronicVariant<unsigned int> hd,tl;
    ReOrderBuffer():hd(1),tl(1){}
};
inline ReOrderBuffer rob;

class Memory
{
    std::unordered_map<unsigned int, unsigned int> mem;
public:
    chronicVariant<unsigned int> addr,clk,val[16],bytes,output[16];// bytes->bytes to write
    chronicVariant<bool> typ,busy,ok;// typ->true read;false write
    void work()
    {
        if(rob.flush.get())
        {
            if(typ.get())
            {
                ok.set(false);
                busy.set(false);
            }
            return ;
        }
        if(busy.get()&&Clk-clk.get()>=3&&!ok.get())
        {
            if(typ.get())
                for(int i=0;i<16;i++)
                    output[i].set(mem[addr.get()+i]);
            else
                for(int i=0;i<bytes.get();i++)
                    mem[addr.get()+i]=val[i].get();
            if(typ.get())
                ok.set(true);
            else
            {
                ok.set(false);
                busy.set(false);
            }
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
class FunctionalUnit
{
public:
    chronicVariant<bool> busy[ALUcnt],ok[ALUcnt];
    chronicVariant<instruction> op[ALUcnt];
    chronicVariant<int> s1[ALUcnt],s2[ALUcnt];
    chronicVariant<unsigned int> ID[ALUcnt];
    chronicVariant<int> output[ALUcnt];
};
inline FunctionalUnit ALU,AGU,BPU;

constexpr int LSBcnt=8;
class LoadStoreBuffer
{
public:
    chronicVariant<bool> ok[LSBcnt];
    chronicVariant<unsigned int> hd,tl;
    chronicVariant<instruction> op[LSBcnt];
    chronicVariant<unsigned int> addr[LSBcnt];
    chronicVariant<unsigned int> qa[LSBcnt]; // address waiting for which item
    chronicVariant<unsigned int> data[LSBcnt];
    chronicVariant<unsigned int> qd[LSBcnt]; // data wating for which item
    chronicVariant<unsigned int> output[LSBcnt];
    chronicVariant<unsigned int> robID[LSBcnt];
};
inline LoadStoreBuffer lsb;

#endif
