#include<iostream>
#include"decoder.hpp"
void work()
{
    while(true)
    {
        tick();
        // some units work here.
        IFC.work();
        decoderWork();
        Imem.work();
        if(Clk==10000)
            break;
    }
}
int main()
{
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