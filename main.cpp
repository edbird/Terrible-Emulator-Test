
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <stdexcept>


// MOVEL
//
// movel reg value (move value into register reg)
// movel reg address (move value at address in mem to register reg)

/*
enum INSTRUCTION : uint64_t
{

    NOOP        = 0x00000000,
    MOVEL       = 0x00000001, // MOVEL address value
    IO_WRITE    = 0x00000002
}*/


typedef uint64_t INSTRUCTION_t;
namespace INSTRUCTION
{
    const INSTRUCTION_t NOOP          = 0x00000000;
    const INSTRUCTION_t MOVEL         = 0x00000001;
    const INSTRUCTION_t JMP           = 0x00000002;
    const INSTRUCTION_t IO_WRITE      = 0x00000003;
    
    const INSTRUCTION_t REG_ADDR0     = 0x00000010;
    const INSTRUCTION_t REG_ADDR1     = 0x00000011;
    const INSTRUCTION_t REG_ADDR2     = 0x00000012;
    const INSTRUCTION_t REG_ADDR3     = 0x00000013;
    const INSTRUCTION_t REG_ADDR4     = 0x00000014;
    const INSTRUCTION_t REG_ADDR5     = 0x00000015;
    const INSTRUCTION_t REG_ADDR6     = 0x00000016;
    const INSTRUCTION_t REG_ADDR7     = 0x00000017;
}


class CPU
{

   
public:

    // stack counter
    uint64_t sc;
    
    // program counter
    uint64_t pc;
    // instruction width (8 byte) (also size of values etc)
    const uint64_t INSTRUCTION_SIZE{8};
    
    // general purpose registers
    const static uint64_t NUM_REG{8};
    uint64_t reg[NUM_REG];
    
    // memory (32k)
    const static uint64_t MEM_SIZE{32 * 1024};
    uint8_t mem[MEM_SIZE];
    
    void init()
    {
        sc = 0;
        pc = 0;
        
        for(uint64_t i{0}; i < NUM_REG; ++ i)
            reg[i] = 0;
            
        
    }
    
    void mem_set(const uint64_t addr, const uint64_t val)
    {
        //for(uint64_t i{0}; i < 8; ++ i)
        //    mem[addr + i] = (uint8_t)(0x000000FF & (val >> (8 * (7 - i))));
        
        mem[addr + 7] = (uint8_t)(0x000000FF & (val >> (8 * 7)));
        mem[addr + 6] = (uint8_t)(0x000000FF & (val >> (8 * 6)));
        mem[addr + 5] = (uint8_t)(0x000000FF & (val >> (8 * 5)));
        mem[addr + 4] = (uint8_t)(0x000000FF & (val >> (8 * 4)));
        mem[addr + 3] = (uint8_t)(0x000000FF & (val >> (8 * 3)));
        mem[addr + 2] = (uint8_t)(0x000000FF & (val >> (8 * 2)));
        mem[addr + 1] = (uint8_t)(0x000000FF & (val >> (8 * 1)));
        mem[addr + 0] = (uint8_t)(0x000000FF & (val >> (8 * 0)));
    }
    
    void mem_dump(std::ostream &os, const uint64_t addr, const uint64_t size)
    {
        os << std::hex;
        for(uint64_t i{addr}; i < addr + size; ++ i)
        {
            for(uint64_t j{0}; j < 8; ++ j)
            {
                uint64_t k{8 * i + j};
                uint8_t v{mem[k]};
                os << k << ": " << (uint64_t)v << '\n';
            }
        }
        os << std::endl;
    }
    
    void advance()
    {
        // all instructions / values have same allignment
        pc += INSTRUCTION_SIZE;
    }
    
    uint64_t mem_load_64(uint64_t addr)
    {
        //std::cout << "mem_load_64: addr=" << addr << " offset=" << addr / 8;
        uint64_t offset{addr / 8};
        uint64_t *memory{(uint64_t*)(void*)&mem[0]};
    
        //std::cout << " memory=" << memory[offset] << std::endl;
        /*std::cout << "mem:\n" << (int)mem[0] << "\n"\
                              << (int)mem[1] << "\n"\
                              << (int)mem[2] << "\n"\
                              << (int)mem[3] << "\n"\
                              << (int)mem[4] << "\n"\
                              << (int)mem[5] << "\n"\
                              << (int)mem[6] << "\n"\
                              << (int)mem[7] << "\n";
        std::cout << std::endl;
        */
        
        //std::cout << (void*)&(mem[0]) << " =? " << memory << std::endl;
        //std::cout << "diff: " << (uint64_t*)memory - (uint64_t*)(&mem[0]) << std::endl;
    
        uint64_t load{memory[offset]};
        return load;
    }
    
    int64_t mem_load_64_signed(uint64_t addr)
    {
        int64_t *memory{(int64_t*)(void*)&mem[0]};
        int64_t load{memory[addr / 8]};
        return load;
    }
    
    void next()
    {
        if(pc < MEM_SIZE - 1)
        {
            INSTRUCTION_t load{mem_load_64(pc)};
            //std::cout << "load=" << load << "pc=" << pc << std::endl;
            advance();
            exec(load);
        }
        else
        {
            throw "PC OVERFLOW";
        }
    }
    
    void exec(const INSTRUCTION_t ins)
    {
        switch(ins)
        {
            case INSTRUCTION::NOOP:
            {
                //std::cout << "exec NOOP" << std::endl;
                //advance();
                break;
            }
                
            case INSTRUCTION::MOVEL:
            {
                //std::cout << "exec MOVEL" << std::endl;
                //advance();
                uint64_t reg_addr{mem_load_64(pc)}; // *(uint64_t*)(mem + pc)
                advance();
                uint64_t value{mem_load_64(pc)};
                advance();
                //uint64_t addr{*(uint64_t*)(mem + pc)};
                reg[reg_addr - INSTRUCTION::REG_ADDR0] = value;
                break;
            }
               
            case INSTRUCTION::JMP:
            {
                //std::cout << "exec JMP" << std::endl;
                //uint64_t offset{mem_load_64(pc)};
                //int64_t signed_offset{offset};
                int64_t signed_offset{mem_load_64_signed(pc)};
                if(signed_offset < 0)
                    pc -= (uint64_t)abs(signed_offset);
                else
                    pc += (uint64_t)signed_offset;
                //std::cout << std::dec << signed_offset << " so" << std::hex << std::endl;
                advance();
                break;
            }
            
            case INSTRUCTION::IO_WRITE:
            {
                //std::cout << "exec IO_WRITE" << std::endl;
                std::cout << reg[0] << std::endl;
                //advance();
                break;
            }
            
            default:
            {
                std::cout << std::hex << std::endl;
                std::cout << "DONT KNOW: " << ins << "pc=" << pc << std::endl;
                throw "internal CPU instruction error";
            }
        }
    }
    
    void run()
    {
        for(;;)
            next();
    }

};

std::string strip_comment(const std::string& str)
{
    std::size_t found{str.find(';')};
    if(found != std::string::npos)
    {
        return str.substr(0, found);
    }
    else
    {
        return str;
    }
}

std::string asm_string_split_next_instruction(const std::string& str)
{
    std::size_t found{str.find(' ')};
    if(found != std::string::npos)
    {
        return str.substr(0, found);
    }
    else
    {
        return str;
    }
}

bool is_value(std::string str)
{
    for(std::size_t i{0}; i < str.size(); ++ i)
    {
        if(str[i] < '0' && str[i] > '9')
        {
            if(i == 0 && str[i] != '-')
            {    
                return false;
            }
        }
    }
      
    return true;
}

void assem(const std::string& filename)
{
    std::ifstream is(filename.c_str());
    if(is.is_open())
    {
        CPU cpu;
        cpu.init();
        uint64_t addr{0};
    
        while(is.good() && !is.eof())
        {
            std::string line;
            getline(is, line);
            std::string asm_{strip_comment(line)};
            std::cout << asm_ << std::endl;
            
            std::size_t asm_index{0};
            for(;;)
            {
                std::string asm_s{asm_string_split_next_instruction(asm_.substr(asm_index, asm_.size() - asm_index))};
                asm_index += asm_s.size() + 1;
                
                //std::cout << "asm_s=" << asm_s << std::endl;
                
                // convert asm instruction from source code (string) to asm (binary)
                uint64_t asm_v{0};
                if(asm_s == "NOOP")
                {
                    asm_v = INSTRUCTION::NOOP;
                    //std::cout << "instruction is NOOP" << std::endl;
                }
                else if(asm_s == "MOVEL")
                {
                    asm_v = INSTRUCTION::MOVEL;
                    //std::cout << "instruction is MOVEL" << std::endl;
                }
                else if(asm_s == "IO_WRITE")
                {
                    asm_v = INSTRUCTION::IO_WRITE;
                    //std::cout << "instruction is IO_WRITE" << std::endl;
                }
                else if(asm_s == "JMP")
                {
                    asm_v = INSTRUCTION::JMP;
                    //std::cout << "instruction is JMP" << std::endl;
                }
                else if(asm_s.size() == 2 && asm_s[0] == 'r')
                {
                    //std::cout << "instruction is r" << std::endl;
                    //std::cout << ":: = " << (int)(asm_s[1] - '0') << std::endl;
                    uint64_t reg_index{(uint64_t)(asm_s[1] - '0')};
                    if(reg_index < cpu.NUM_REG)
                    {
                        asm_v = INSTRUCTION::REG_ADDR0 + reg_index;
                    }
                    else
                    {
                        throw "invalid register index";
                    }
                    //std::cout << "register is " << asm_v << std::endl;
                }
                else if(is_value(asm_s))
                {
                    asm_v = std::atoll(asm_s.c_str());
                    //std::cout << "value is " << asm_v << std::endl;
                }
                else
                {
                    throw "invalid asm code";
                }
                
                cpu.mem_set(addr, asm_v);
                addr += cpu.INSTRUCTION_SIZE;
                //std::cout << "addr = " << addr << std::endl;
                
                //std::cin.get();
                
                if(asm_index >= asm_.size())
                    break;
            }
        }
        
        cpu.mem_dump(std::cout, 0, 0x0F);
        
        try
        {
            cpu.run();
        }
        catch(std::string &e)
        {
            std::cerr << e << std::endl;
        }
    }
    else
    {
        //throw "file " << filename << "does not exist";
        throw "file does not exist";
    }
}

    
    /*
    void mem_set(char mem[], const uint64_t addr, const uint64_t val)
    {
        //for(uint64_t i{0}; i < 8; ++ i)
        //    mem[addr + i] = (uint8_t)(0x000000FF & (val >> (8 * (7 - i))));
        
        mem[8 * addr + 7] = (uint8_t)(0x000000FF & (val >> (8 * 7)));
        mem[8 * addr + 6] = (uint8_t)(0x000000FF & (val >> (8 * 6)));
        mem[8 * addr + 5] = (uint8_t)(0x000000FF & (val >> (8 * 5)));
        mem[8 * addr + 4] = (uint8_t)(0x000000FF & (val >> (8 * 4)));
        mem[8 * addr + 3] = (uint8_t)(0x000000FF & (val >> (8 * 3)));
        mem[8 * addr + 2] = (uint8_t)(0x000000FF & (val >> (8 * 2)));
        mem[8 * addr + 1] = (uint8_t)(0x000000FF & (val >> (8 * 1)));
        mem[8 * addr + 0] = (uint8_t)(0x000000FF & (val >> (8 * 0)));
    }

    uint64_t mem_load_64(char mem[], uint64_t addr)
    {
        std::cout << "mem_load_64: addr=" << addr << " offset=" << addr / 8;
        uint64_t offset{addr / 8};
        uint64_t *memory{(uint64_t*)(void*)&mem[0]};
    
        std::cout << " memory=" << memory[offset] << std::endl;
        std::cout << "mem:\n" << (int)mem[0] << "\n"\
                              << (int)mem[1] << "\n"\
                              << (int)mem[2] << "\n"\
                              << (int)mem[3] << "\n"\
                              << (int)mem[4] << "\n"\
                              << (int)mem[5] << "\n"\
                              << (int)mem[6] << "\n"\
                              << (int)mem[7] << "\n";
        std::cout << std::endl;
    
        std::cout << (void*)&(mem[0]) << " =? " << memory << std::endl;
        std::cout << "diff: " << (uint64_t*)memory - (uint64_t*)(&mem[0]) << std::endl;
    
        uint64_t load{memory[offset]};
        return load;
    }
    
    void mem_dump(std::ostream &os, char mem[], const uint64_t addr, const uint64_t size)
    {
        os << std::hex;
        for(uint64_t i{addr}; i < addr + size; ++ i)
        {
            for(uint64_t j{0}; j < 8; ++ j)
            {
                uint64_t k{8 * i + j};
                uint8_t v{mem[k]};
                os << k << ": " << (uint64_t)v << '\n';
            }
        }
        os << std::endl;
    }
    */
    
int main()
{
    
    /*
    char mem[16];
    std::cout << (void*)&(mem[0]) << std::endl;
    
    mem_set(mem, 0, 0xff);
    mem_set(mem, 1, 0xee);
    mem_dump(std::cout, mem, 0, 2);
    
    uint64_t *memory{(uint64_t*)(void*)&mem[0]};
    uint64_t load{memory[0]};
    std::cout << "load=" << load << std::endl;
        
    return 1;
    */
    
    
    assem("first.asm");
    
    return 0;
}
