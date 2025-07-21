// A decoder of machine code, translating them into assembly language

#ifndef DECODER_H
#define DECODER_H

constexpr bool pseudo=false;

namespace Decoder
{
    inline std::string regName(const int id)
    {
        static std::string res[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",
        "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7",
        "s8","s9","s10","s11","t3","t4","t5","t6"};
        if(id>=32||id<0)
            return "uk-"+std::to_string(id);
        return res[id];
    }
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
        friend std::ostream& operator<<(std::ostream& os,const instruction& other)
        {
            const std::string& s=other.op;
            if(pseudo)
            {
                if(s=="beq"&&other.p2==0)
                    return os<<"beqz"<<" "<<regName(other.p1)<<" "<<other.p0;
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
            else if(s=="lb"||s=="lbu"||s=="lh"||s=="lhu"||s=="lw"||s=="sb"||s=="sh"||s=="sw")
                os<<" "<<regName(other.p0)<<" "<<other.p2<<"("<<regName(other.p1)<<")";
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
    };

    inline unsigned int hexCombiner(char** hex)
    {
        auto hexToInt=[](const char ch)->unsigned int{return (ch>='0'&&ch<='9')?ch-'0':ch-'A'+10;};
        unsigned int result=0;
        for(int i=3;i>=0;i--)
            for(int j=0;j<=1;j++)
                result=(result<<4)|hexToInt(hex[i][j]);
        return result;
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
            std::string op;
            if(funct3==0b000)
                op=funct7?"sub":"add";
            else if(funct3==0b111)
                op="and";
            else if(funct3==0b110)
                op="or";
            else if(funct3==0b100)
                op="xor";
            else if(funct3==0b001)
                op="sll";
            else if(funct3==0b101)
                op=funct7?"sra":"srl";
            else if(funct3==0b010)
                op="slt";
            else if(funct3==0b011)
                op="sltu";
            return {op,rd,rs1,rs2};
        }
        if(opcode==0b0010011)// type-I/I*(Arithmetic)
        {
            const int funct3=(ins>>12)&7;
            const int funct7=(ins>>25)&127;
            const int rd=(ins>>7)&31;
            const int rs1=(ins>>15)&31;
            int imm=(ins>>20)&4095;
            std::string op;
            if(funct3==0b000)
                op="addi";
            else if(funct3==0b111)
                op="andi";
            else if(funct3==0b110)
                op="ori";
            else if(funct3==0b100)
                op="xori";
            else if(funct3==0b001)
                op="slli",imm&=15;
            else if(funct3==0b101)
                op=funct7?"srai":"srli";
            else if(funct3==0b010)
                op="slti";
            else if(funct3==0b011)
                op="sltiu";
            return {op,rd,rs1,imm};
        }
        if(opcode==0b0000011)// type-I(Memory)
        {
            const int funct3=(ins>>12)&7;
            const int rd=(ins>>7)&31;
            const int rs1=(ins>>15)&31;
            const int imm=(ins>>20)&4095;
            std::string op;
            if(funct3==0b000)
                op="lb";
            else if(funct3==0b100)
                op="lbu";
            else if(funct3==0b001)
                op="lh";
            else if(funct3==0b101)
                op="lhu";
            else if(funct3==0b010)
                op="lw";
            return {op,rd,rs1,imm};
        }
        if(opcode==0b0100011)// type-S
        {
            const int funct3=(ins>>12)&7;
            const int rs1=(ins>>15)&31;
            const int rs2=(ins>>20)&31;
            const int imm=((ins>>7)&31)|(((ins>>25)&127)<<5);
            std::string op;
            if(funct3==0b000)
                op="sb";
            else if(funct3==0b001)
                op="sh";
            else if(funct3==0b010)
                op="sw";
            return {op,imm,rs1,rs2};
        }
        if(opcode==0b1100011)// type-B
        {
            const int funct3=(ins>>12)&7;
            const int rs1=(ins>>15)&31;
            const int rs2=(ins>>20)&31;
            const int imm=(((ins>>8)&15)<<1)|(((ins>>25)&63)<<5)|(((ins>>7)&1)<<11)|(((ins>>31)&1)<<12);
            std::string op;
            if(funct3==0b000)
                op="beq";
            else if(funct3==0b101)
                op="bge";
            else if(funct3==0b111)
                op="bgeu";
            else if(funct3==0b100)
                op="blt";
            else if(funct3==0b110)
                op="bltu";
            else if(funct3==0b001)
                op="bne";
            return {op,imm,rs1,rs2};
        }
        if(opcode==0b1101111)// type-J
        {
            const int rd=(ins>>7)&31;
            const int imm=(((ins>>12)&255)<<12)|(((ins>>20)&1)<<11)|(((ins>>21)&1023)<<1)|(((ins>>31)&1)<<20);
            const std::string op="jal";
            return {op,rd,imm,0};
        }
        if(opcode==0b1100111)// type-I(Control)
        {
            const int rd=(ins>>7)&31;
            const int rs1=(ins>>15)&31;
            const int imm=(ins>>20)&4095;
            const std::string op="jalr";
            return {op,rd,rs1,imm};
        }
        if(opcode==0b0010111)// type-U(auipc)
        {
            const int rd=(ins>>7)&31;
            const int imm=((ins>>12)&1048575)<<12;
            const std::string op="auipc";
            return {op,rd,imm,0};
        }
        if(opcode==0b0110111)// type-U(lui)
        {
            const int rd=(ins>>7)&31;
            const int imm=((ins>>12)&1048575)<<12;
            const std::string op="lui";
            return {op,rd,imm,0};
        }
        if(opcode==0b1110011)// type-I(Other)
        {
            const auto imm=(ins>>20)&4095;
            std::string op;
            if(!imm)
                op="ebreak";
            else
                op="ecall";
            return {op,0,0,0};
        }
        return {"",0,0,0};
    }
    inline instruction launchDecoder(char** hex){return instructionDecoder(hexCombiner(hex));}
}

#endif
