#include<iostream>
#include<bitset>
#include"decoder.hpp"
#include"memory.hpp"
#include"alu.hpp"
#include"lsb.hpp"
#include"rob.hpp"
#include"rs.hpp"
#include"coder.hpp"
void work()
{
    while(true)
    {
        tick();
        // some units work here.
        // the sequence can be changed arbitrarily.
        aguWork();
        Dmem.work();
        robWork();
        lsbWork();
        rsWork();
        decoderWork();
        bpuWork();
        IFCwork();
        aluWork();
        Imem.work();
        rf.reg[0].set(0);
        rf.robID[0].set(0);
        rf.busy[0].set(false);
    }
}
int main()
{
    // std::string input;
    // std::string result;
    // while(true)
    // {
    //     std::getline(std::cin,input);
    //     result+=hexize(coder(strToInstruction(input)))+" ";
    //     std::cout<<result<<std::endl;
    //     std::cout<<instructionDecoder(coder(strToInstruction(input)))<<std::endl;
    // }
    char ch[30];
    unsigned int addr=0;
    while(scanf("%s",ch)!=EOF)
    {
        if(ch[0]=='@')
        {
            addr=0;
            const int len=strlen(ch);
            for(int i=1;i<len;i++)
                addr=(addr<<4)+singleHexToDec(ch[i]);
            continue;
        }
        if(ch[0]=='!')
            break;
        Imem.init(addr,(singleHexToDec(ch[0])<<4)|singleHexToDec(ch[1]));
        addr++;
    }
    work();

    return 0;
}