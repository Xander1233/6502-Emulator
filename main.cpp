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

    void Write(Byte value, u32 Address, u32& ticks) {
        Data[Address] = value;
        ticks--;
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

        // Transfer Accumulator to x
        INS_TAX = 0xAA, // 2 ticks

        // Transfer Accumulator to y
        INS_TAY = 0xA8, // 2 ticks

        // Transfer x to Accumulator
        INS_TXA = 0x8A, // 2 ticks

        // Transfer y to Accumulator
        INS_TYA = 0x98, // 2 ticks

        // Transfer stack pointer to x
        INS_TSX = 0xBA, // 2 ticks

        // Transfer x to stack pointer
        INS_TXS = 0x9A, // 2 ticks

        // Push Accumulator
        INS_PHA = 0x48, // 3 ticks

        // Pull Accumulator
        INS_PLA = 0x68, // 4 ticks

        // Increment x
        INS_INX = 0xE8, // 2 ticks

        // Increment y
        INS_INY = 0xC8, // 2 ticks

        // Decrement x
        INS_DEX = 0xCA, // 2 ticks

        // Decrement y
        INS_DEY = 0x88, // 2 ticks

        // No operation
        INS_NOP = 0xEA, // 2 ticks

        // Return from Subroutine
        INS_RTS = 0x60, // 6 ticks

        // Set Carry Flag
        INS_SEC = 0x38, // 2 ticks

        // Set Decimal Flag
        INS_SED = 0xF8, // 2 ticks

        // Set Interrupt Disable
        INS_SEI = 0x78, // 2 ticks

        // Clear Carry Flag
        INS_CLC = 0x18, // 2 ticks

        // Clear Decimal Mode
        INS_CLD = 0xD8, // 2 ticks

        // Clear Interrupt Disable
        INS_CLI = 0x58, // 2 ticks

        // Clear Overflow Flag
        INS_CLV = 0xB8, // 2 ticks

        // Jumps
        INS_JMP = 0x4C, // 3 ticks
        INS_JSR = 0x20; // 6 ticks

    void Reset(MEMORY& memory) {
        program_counter = 0xFFF1;
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

                case INS_TSX: {
                    x = stack_pointer;
                    ticks--;
                    LDXSetFlags();
                } break;

                case INS_TAX: {
                    x = a;
                    ticks--;
                    LDXSetFlags();
                } break;

                case INS_TAY: {
                    y = a;
                    ticks--;
                    LDYSetFlags();
                } break;

                case INS_TXA: {
                    a = x;
                    ticks--;
                    LDASetFlags();
                } break;

                case INS_TXS: {
                    stack_pointer = x;
                    ticks--;
                } break;

                case INS_TYA: {
                    a = y;
                    ticks--;
                    LDASetFlags();
                } break;

                case INS_PHA: {
                    memory.WriteWord(a << 8, stack_pointer, ticks);
                    stack_pointer++;
                } break;

                case INS_PLA: {
                    a = memory[stack_pointer];
                    ticks--;
                    memory[stack_pointer] = 0;
                    ticks--;
                    stack_pointer--;
                    LDASetFlags();
                } break;

                case INS_INX: {
                    x++;
                    ticks--;
                    LDXSetFlags();
                } break;

                case INS_INY: {
                    y++;
                    ticks--;
                    LDYSetFlags();
                } break;

                case INS_NOP: {
                    ticks--;
                } break;

                case INS_SEC: {
                    carry = 1;
                    ticks--;
                } break;

                case INS_SED: {
                    decimal = 1;
                    ticks--;
                } break;

                case INS_SEI: {
                    interrupt = 1;
                    ticks--;
                } break;

                case INS_CLC: {
                    carry = 0;
                    ticks--;
                } break;

                case INS_CLD: {
                    decimal = 0;
                    ticks--;
                } break;

                case INS_CLI: {
                    interrupt = 0;
                    ticks--;
                } break;

                case INS_CLV: {
                    overflow = 0;
                    ticks--;
                } break;

                case INS_DEX: {
                    x--;
                    ticks--;
                    LDXSetFlags();
                } break;

                case INS_DEY: {
                    y--;
                    ticks--;
                    LDYSetFlags();
                } break;

                case INS_JMP: {
                    Word newAddress = FetchWord(ticks, memory);
                    program_counter = newAddress;
                } break;

                case INS_RTS: {
                    program_counter = stack_pointer;
                    Word NewProgramCounter = FetchWord(ticks, memory);
                    memory.WriteWord(0, stack_pointer, ticks);
                    program_counter = NewProgramCounter;
                    ticks--;
                    stack_pointer -= 2;
                } break;

                case INS_JSR: {
                    Word SubRoutineAddress = FetchWord(ticks, memory);
                    memory.WriteWord(program_counter - 1, stack_pointer, ticks);
                    stack_pointer++;
                    program_counter = SubRoutineAddress;
                } break;

                default: {
                    printf("Instruction not handled %d\n", instruction);
                } break;
            }
            if (program_counter < 0xFF00) {
                break;
            }
        }
    }
};

int main() {
    MEMORY memory;
    CPU6502 cpu;
    cpu.Reset(memory);

    // inline program (test instructions (hard coded into memory)) - Start

    memory[0xFFF1] = CPU6502::INS_LDA_IM; // 2 ticks
    memory[0xFFF2] = 0x26;
    memory[0xFFF3] = CPU6502::INS_TAX; // 2 ticks
    memory[0xFFF4] = CPU6502::INS_INX; // 2 ticks
    memory[0xFFF5] = CPU6502::INS_JMP; // 3 ticks
    memory[0xFFF6] = 0x01;
    memory[0xFFF7] = 0xFF;
    memory[0xFF01] = CPU6502::INS_TAY; // 2 ticks
    memory[0xFF02] = CPU6502::INS_DEY; // 2 ticks
    memory[0xFF03] = CPU6502::INS_DEY; // 2 ticks

    // inline program (test instructions (hard coded into memory)) - End

    cpu.Execute(15, memory);

    if (cpu.program_counter < 0xFF00) {
        printf("Program counter overflow. Exit");
        return 1;
    }

    printf("a register: %d\nx register: %d\ny register: %d", cpu.a, cpu.x, cpu.y);

    return 0;
};
