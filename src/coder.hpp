#ifndef CODER_H
#define CODER_H

#include "base.hpp"

inline char decToHexSingle(const unsigned int dec)
{
    if(dec<10)
        return dec+'0';
    return dec-10+'A';
}
inline std::string hexize(unsigned int ins)
{
    std::string res;
    for(int i=0;i<4;i++)
    {
        if(i)
            res.push_back(' ');
        res.push_back(decToHexSingle((ins>>4)&15));
        res.push_back(decToHexSingle(ins&15));
        ins>>=8;
    }
    return res;
}

unsigned int coder(const instruction a)
{
    const unsigned int op=static_cast<int>(a.op);
    if(op>=1&&op<=10)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<27)>>27;
        if(op==1)
            return (0b0110011)|(p0<<7)|(0b000<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==2)
            return (0b0110011)|(p0<<7)|(0b000<<12)|(p1<<15)|(p2<<20)|(0b0100000<<25);
        if(op==3)
            return (0b0110011)|(p0<<7)|(0b111<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==4)
            return (0b0110011)|(p0<<7)|(0b110<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==5)
            return (0b0110011)|(p0<<7)|(0b100<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==6)
            return (0b0110011)|(p0<<7)|(0b001<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==7)
            return (0b0110011)|(p0<<7)|(0b101<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==8)
            return (0b0110011)|(p0<<7)|(0b101<<12)|(p1<<15)|(p2<<20)|(0b0100000<<25);
        if(op==9)
            return (0b0110011)|(p0<<7)|(0b010<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==10)
            return (0b0110011)|(p0<<7)|(0b011<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
    }
    if((op>=11&&op<=14)||(op>=18&&op<=19))
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<20)>>20;
        if(op==11)
            return (0b0010011)|(p0<<7)|(0b000<<12)|(p1<<15)|(p2<<20);
        if(op==12)
            return (0b0010011)|(p0<<7)|(0b111<<12)|(p1<<15)|(p2<<20);
        if(op==13)
            return (0b0010011)|(p0<<7)|(0b110<<12)|(p1<<15)|(p2<<20);
        if(op==14)
            return (0b0010011)|(p0<<7)|(0b100<<12)|(p1<<15)|(p2<<20);
        if(op==18)
            return (0b0010011)|(p0<<7)|(0b010<<12)|(p1<<15)|(p2<<20);
        if(op==19)
            return (0b0010011)|(p0<<7)|(0b010<<12)|(p1<<15)|(p2<<20);
    }
    if(op>=15&&op<=17)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<27)>>27;
        if(op==15)
            return (0b0010011)|(p0<<7)|(0b001<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==16)
            return (0b0010011)|(p0<<7)|(0b101<<12)|(p1<<15)|(p2<<20)|(0b0000000<<25);
        if(op==17)
            return (0b0010011)|(p0<<7)|(0b101<<12)|(p1<<15)|(p2<<20)|(0b0100000<<25);
    }
    if(op>=20&&op<=24)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<20)>>20;
        if(op==20)
            return (0b0000011)|(p0<<7)|(0b000<<12)|(p1<<15)|(p2<<20);
        if(op==21)
            return (0b0000011)|(p0<<7)|(0b100<<12)|(p1<<15)|(p2<<20);
        if(op==22)
            return (0b0000011)|(p0<<7)|(0b001<<12)|(p1<<15)|(p2<<20);
        if(op==23)
            return (0b0000011)|(p0<<7)|(0b101<<12)|(p1<<15)|(p2<<20);
        if(op==24)
            return (0b0000011)|(p0<<7)|(0b010<<12)|(p1<<15)|(p2<<20);
    }
    if(op>=25&&op<=27)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<20)>>20,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<27)>>27;
        const unsigned int p0H=p0>>5,p0L=p0&31;
        if(op==25)
            return (0b0100011)|(p0L<<7)|(0b000<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==26)
            return (0b0100011)|(p0L<<7)|(0b001<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==27)
            return (0b0100011)|(p0L<<7)|(0b010<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
    }
    if(op>=28&&op<=33)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<19)>>19,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<27)>>27;
        const unsigned int p0H=((p0>>5)&63)|((p0&(1<<12))>>6),p0L=(p0&31)|((p0&(1<<11))>>11);
        if(op==28)
            return (0b1100011)|(p0L<<7)|(0b000<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==29)
            return (0b1100011)|(p0L<<7)|(0b101<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==30)
            return (0b1100011)|(p0L<<7)|(0b111<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==31)
            return (0b1100011)|(p0L<<7)|(0b100<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==32)
            return (0b1100011)|(p0L<<7)|(0b110<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
        if(op==33)
            return (0b1100011)|(p0L<<7)|(0b001<<12)|(p1<<15)|(p2<<20)|(p0H<<25);
    }
    if(op==34)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<11)>>11;
        const unsigned int pr=((p1>>12)&255)|((p1&(1<<11))>>3)|(((p1>>1)&1023)<<9)|((p1&(1<<20))>>1);
        return (0b1101111)|(p0<<7)|(pr<<12);
    }
    if(op==35)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<27)>>27,
            p2=static_cast<unsigned int>(a.p2<<20)>>20;
        return (0b1100111)|(p0<<7)|(0b000<<12)|(p1<<15)|(p2<<20);
    }
    if(op>=36&&op<=37)
    {
        const unsigned int p0=static_cast<unsigned int>(a.p0<<27)>>27,p1=static_cast<unsigned int>(a.p1<<12)>>12;
        if(op==36)
            return (0b0010111)|(p0<<7)|(p1<<12);
        if(op==37)
            return (0b0110111)|(p0<<7)|(p1<<12);
    }
    if(op==38)
        return 0b000000000000001110011;
    if(op==39)
        return 0b100000000000001110011;
    return 0;
}

inline std::string tokenTaker(std::string& a)
{
    int l=0;
    while(l!=a.size()&&(a[l]==' '||a[l]=='('||a[l]==')'||a[l]==','||a[l]=='\t'||a[l]=='\n'))
        l++;
    int r=l;
    while(r!=a.size()&&a[r]!=' '&&a[r]!='('&&a[r]!=')'&&a[r]!=','&&a[r]!='\t'&&a[r]!='\n')
        r++;
    auto res=a.substr(l,r-l);
    a=a.substr(r);
    return res;
}

inline int regID(const std::string& regName)
{
    static constexpr std::string res[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",
    "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"};
    for(int i=0;i<32;i++)
        if(res[i]==regName)
            return i;
    return 32;
}

inline int insID(const std::string& insName)
{
    static constexpr std::string res[]={"","add","sub","and","or","xor","sll","srl","sra","slt","sltu",
    "addi","andi","ori","xori","slli","srli","srai","slti","sltiu","lb","lbu","lh","lhu",
    "lw","sb","sh","sw","beq","bge","bgeu","blt","bltu","bne","jal","jalr","auipc",
    "lui","ebreak","ecall"};
    for(int i=0;i<40;i++)
        if(res[i]==insName)
            return i;
    return 40;
}

inline instruction strToInstruction(std::string str)
{
    const std::string OP=tokenTaker(str),s1=tokenTaker(str),s2=tokenTaker(str),s3=tokenTaker(str);
    if(pseudo)
    {
        if(OP=="beqz")
            return {28,std::stoi(s2),regID(s1),0};
        if(OP=="bltz")
            return {31,std::stoi(s2),regID(s1),0};
        if(OP=="bgez")
            return {29,std::stoi(s2),regID(s1),0};
        if(OP=="blez")
            return {29,std::stoi(s2),0,regID(s1)};
        if(OP=="bnez")
            return {33,std::stoi(s2),regID(s1),0};
        if(OP=="j")
            return {34,0,std::stoi(s1),0};
        if(OP=="jal"&&s2.empty())
            return {33,1,std::stoi(s1),0};
        if(OP=="jr")
            return {35,0,regID(s1),0};
        if(OP=="li")
            return {11,regID(s1),0,std::stoi(s2)};
        if(OP=="mv")
            return {11,regID(s1),regID(s2),0};
        if(OP=="neg")
            return {2,regID(s1),0,regID(s2)};
        if(OP=="nop")
            return {11,0,0,0};
        if(OP=="not")
            return {14,regID(s1),regID(s2),-1};
        if(OP=="ret")
            return {35,0,1,0};
    }
    int op=insID(OP);
    if(op<0||op>=40)
        return {0,0,0,0};
    if(op<=10)
        return {op,regID(s1),regID(s2),regID(s3)};
    if(op<=19)
        return {op,regID(s1),regID(s2),std::stoi(s3)};
    if(op<=24)
        return {op,regID(s1),regID(s3),std::stoi(s2)};
    if(op<=27)
        return {op,std::stoi(s2),regID(s3),regID(s1)};
    if(op<=33)
        return {op,std::stoi(s3),regID(s1),regID(s2)};
    if(op<=34)
        return {op,regID(s1),std::stoi(s2),0};
    if(op<=35)
        return {op,regID(s1),regID(s2),std::stoi(s3)};
    if(op<=37)
        return {op,regID(s1),std::stoi(s2),0};
    return {op,0,0,0};
}

#endif