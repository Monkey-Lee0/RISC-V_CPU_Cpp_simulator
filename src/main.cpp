#include<iostream>
#include"decoder.h"
Decoder::instruction ins[5000];// instructions
int cntIns=0;
unsigned int reg[32];// registers
unsigned int pc;// pc pointer
unsigned int mem[100000];// memory
int main()
{
    const auto ch=new char*[4];
    for(int i=0;i<4;i++)
        ch[i]=new char[30];
    int kd=0;
    while(scanf("%s",ch[kd])!=EOF)
    {
        if(ch[kd][0]=='@')
            continue;
        if(ch[kd][0]=='!')
            break;
        kd++;
        if(kd==4)
        {
            ins[++cntIns]=Decoder::launchDecoder(ch);
            if(ins[cntIns].op.empty())
                cntIns--;
            kd=0;
        }
    }
    for(int i=1;i<=cntIns;i++)
        std::cout<<ins[i]<<std::endl;

    return 0;
}