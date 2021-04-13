#include <stdlib.h>
#include <stdio.h>

// Regarding to http://www.obelisk.me.uk/6502/

using Byte = unsigned char; // 8 bit
using Word = unsigned short; // 16 bit
using u32 = unsigned int;

struct MEMORY {
    static constexpr u32 MAX_MEMORY = 1024 * 64;
    Byte Data[MAX_MEMORY];

    void Initialize() {
        for (u32 i = 0; i < MAX_MEMORY; i++) {
            Data[i] = 0;
        }
    }

    Byte operator[] (u32 Address) const {
        return Data[Address];
    }

    Byte& operator[] (u32 Address) {
        return Data[Address];
    }

    void WriteWord(Word value, u32 Address, u32& ticks) {
        Data[Address] = value & 0xFF;
        Data[Address + 1] = (value >> 8);
        ticks -= 2;
    }
};

struct CPU6502 {

    Word program_counter;
    Word stack_pointer;

    Byte a, x, y; // registers

    Byte carry : 1; // Carry flag
    Byte zero : 1; // Zero flag
    Byte interrupt : 1; // Interrupt disable flag
    Byte decimal : 1; // Decimal mode flag
    Byte Break : 1; // Break command flag
    Byte overflow : 1; // Overflow flag
    Byte negative : 1; // Negative flag

    // Instructions
    static constexpr Byte

        // Load A register
        INS_LDA_IM = 0xA9, // 2 ticks
        INS_LDA_ZP = 0xA5, // 3 ticks
        INS_LDA_ZPX = 0xB5, // 4 ticks

        // Load X register
        INS_LDX_IM = 0xA2, // 2 ticks
        INS_LDX_ZP = 0xA6, // 3 ticks
        INS_LDX_ZPY = 0xB6, // 4 ticks

        // Load Y register
        INS_LDY_IM = 0xA0, // 2 ticks
        INS_LDY_ZP = 0xA4, // 3 ticks
        INS_LDY_ZPX = 0xB4, // 4 ticks

        // Store A register
        INS_STA_ZP = 0x85, // 3 ticks
        INS_STA_ZPX = 0x95, // 4 ticks

        // Store X register
        INS_STX_ZP = 0x86, // 3 ticks
        INS_STX_ZPY = 0x96, // 4 ticks

        // Store Y register
        INS_STY_ZP = 0x84, // 3 ticks
        INS_STY_ZPX = 0x94, // 4 ticks

        // Jumps
        INS_JSR = 0x20; // 6 ticks

    void Reset(MEMORY& memory) {
        program_counter = 0xFFCC;
        stack_pointer = 0x1000;
        a = x = y = 0;
        carry = zero = interrupt = decimal = Break = overflow = negative = 0;
        memory.Initialize();
    }

    Byte Fetch(u32& ticks, MEMORY& memory) {
        Byte instruction = memory[program_counter];
        program_counter++;
        ticks--;
        return instruction;
    }

    Byte Read(u32& ticks, Byte address, MEMORY& memory) {
        Byte Data = memory[address];
        ticks--;
        return Data;
    }

    Word FetchWord(u32& ticks, MEMORY& memory) {
        Word Data = memory[program_counter];
        program_counter++;

        Data |= (memory[program_counter] << 8);
        program_counter++;

        ticks -= 2;
        return Data;
    }

    void LDASetFlags() {
        zero = (a == 0);
        negative = (a & 0b10000000) > 0;
    }

    void LDXSetFlags() {
        zero = (x == 0);
        negative = (x & 0b10000000) > 0;
    }

    void LDYSetFlags() {
        zero = (y == 0);
        negative = (y & 0b10000000) > 0;
    }

    void Execute(u32 ticks, MEMORY& memory) {
        while (ticks > 0) {
            Byte instruction = Fetch(ticks, memory);
            switch (instruction) {
                case INS_LDA_IM: {
                    Byte value = Fetch(ticks, memory);
                    a = value;
                    LDASetFlags();
                } break;

                case INS_LDA_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    a = Read(ticks, ZeroPageAddress, memory);
                    LDASetFlags();
                } break;

                case INS_LDA_ZPX: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += x;
                    ticks--;
                    a = Read(ticks, ZeroPageAddress, memory);
                    LDASetFlags();
                } break;

                case INS_LDX_IM: {
                    Byte value = Fetch(ticks, memory);
                    x = value;
                    LDXSetFlags();
                } break;

                case INS_LDX_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    x = Read(ticks, ZeroPageAddress, memory);
                    LDXSetFlags();
                } break;

                case INS_LDX_ZPY: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += y;
                    ticks--;
                    x = Read(ticks, ZeroPageAddress, memory);
                    LDXSetFlags();
                } break;

                case INS_LDY_IM: {
                    Byte value = Fetch(ticks, memory);
                    y = value;
                    LDYSetFlags();
                } break;

                case INS_LDY_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    y = Read(ticks, ZeroPageAddress, memory);
                    LDYSetFlags();
                } break;

                case INS_LDY_ZPX: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += x;
                    ticks--;
                    y = Read(ticks, ZeroPageAddress, memory);
                } break;

                case INS_STA_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    memory.Write(a, ZeroPageAddress, ticks);
                } break;

                case INS_STA_ZPX: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += x;
                    memory.Write(a, ZeroPageAddress, ticks);
                } break;
                
                case INS_STX_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    memory.Write(x, ZeroPageAddress, ticks);
                } break;

                case INS_STX_ZPY: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += y;
                    memory.Write(x, ZeroPageAddress, ticks);
                } break;
                    
                case INS_STY_ZP: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    memory.Write(y, ZeroPageAddress, ticks);
                } break;

                case INS_STY_ZPX: {
                    Byte ZeroPageAddress = Fetch(ticks, memory);
                    ZeroPageAddress += x;
                    memory.Write(y, ZeroPageAddress, ticks);
                }
                    
                case INS_JSR: {
                    Word SubRoutineAddress = FetchWord(ticks, memory);
                    memory.WriteWord(program_counter - 1, stack_pointer, ticks);
                    stack_pointer++;
                    program_counter = SubRoutineAddress;
                    ticks--;
                } break;

                default: {
                    printf("Instruction not handled %d\n", instruction);
                } break;
            }
        }
    }
};

int main() {
    MEMORY memory;
    CPU6502 cpu;
    cpu.Reset(memory);

    // inline program (test instructions (hard coded into memory)) - Start
    memory[0xFFCC] = CPU6502::INS_LDA_IM;
    memory[0xFFCD] = 0x3;
    memory[0xFFCE] = CPU6502::INS_LDX_IM;
    memory[0xFFCF] = 0x9;
    memory[0xFFD0] = CPU6502::INS_LDY_IM;
    memory[0xFFD1] = 0x5;
    // inline program (test instructions (hard coded into memory)) - End

    cpu.Execute(6, memory);

    printf("a register: %d\nx register: %d\ny register: %d", cpu.a, cpu.x, cpu.y);

    return 0;
};
