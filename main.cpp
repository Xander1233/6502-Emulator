#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

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

char asciitolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

void toLowerCase(string& input) {
    string res = "";
    for (int i = 0; i < input.length(); i++) {
        res += asciitolower(input[i]);
    }
    input = res;
}

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

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

        // Logical AND
        INS_AND_IM = 0x29, // 2 ticks
        INS_AND_ZP = 0x25, // 3 ticks
        INS_AND_ZPX = 0x35, // 4 ticks

        // Decrement Memory
        INS_DEC_ZP = 0xC6, // 4 ticks
        INS_DEC_ZPX = 0xD6, // 5 ticks

        // Increment Memory
        INS_INC_ZP = 0xE6, // 4 ticks
        INS_INC_ZPX = 0xF6, // 6 ticks

        // Arithmetic Shift Left
        INS_ASL_ACC = 0x0A, // 2 ticks
        INS_ASL_ZP = 0x06, // 5 ticks
        INS_ASL_ZPX = 0x16, // 6 ticks

        // Logical Shift Right
        INS_LSR_ACC = 0x4A, // 2 ticks
        INS_LSR_ZP = 0x46, // 5 ticks
        INS_LSR_ZPX = 0x56, // 6 ticks

        // Logical Inclusive OR
        INS_ORA_IM = 0x09, // 2 ticks
        INS_ORA_ZP = 0x05, // 3 ticks
        INS_ORA_ZPX = 0x15, // 4 ticks

        // Jumps
        INS_JMP = 0x4C, // 3 ticks
        INS_JSR = 0x20; // 6 ticks


    void interpretInstruction(string instruction, Word& counter, MEMORY& memory, u32& ticks) {
        Byte ins;

        if (instruction.compare("ldaim") == 0) {
            ins = INS_LDA_IM;
            ticks += 2;
        }
        else if (instruction.compare("ldazp") == 0) {
            ins = INS_LDA_ZP;
            ticks += 3;
        }
        else if (instruction.compare("ldazpx") == 0) {
            ins = INS_LDA_ZPX;
            ticks += 4;
        }
        else if (instruction.compare("ldxim") == 0) {
            ins = INS_LDX_IM;
            ticks += 2;
        }
        else if (instruction.compare("ldxzp") == 0) {
            ins = INS_LDX_ZP;
            ticks += 3;
        }
        else if (instruction.compare("ldxzpy") == 0) {
            ins = INS_LDX_ZPY;
            ticks += 4;
        }
        else if (instruction.compare("ldyim") == 0) {
            ins = INS_LDY_IM;
            ticks += 2;
        }
        else if (instruction.compare("ldyzp") == 0) {
            ins = INS_LDY_ZP;
            ticks += 3;
        }
        else if (instruction.compare("ldyzpx") == 0) {
            ins = INS_LDY_ZPX;
            ticks += 4;
        }

        else if (instruction.compare("sta") == 0) {
            ins = INS_STA_ZP;
            ticks += 3;
        }
        else if (instruction.compare("stax") == 0) {
            ins = INS_STA_ZPX;
            ticks += 4;
        }
        else if (instruction.compare("stx") == 0) {
            ins = INS_STX_ZP;
            ticks += 3;
        }
        else if (instruction.compare("stxy") == 0) {
            ins = INS_STX_ZPY;
            ticks += 4;
        }
        else if (instruction.compare("sty") == 0) {
            ins = INS_STY_ZP;
            ticks += 3;
        }
        else if (instruction.compare("styx") == 0) {
            ins = INS_STY_ZPX;
            ticks += 4;
        }

        else if (instruction.compare("tax") == 0) {
            ins = INS_TAX;
            ticks += 2;
        }
        else if (instruction.compare("tay") == 0) {
            ins = INS_TAY;
            ticks += 2;
        }
        else if (instruction.compare("txa") == 0) {
            ins = INS_TXA;
            ticks += 2;
        }
        else if (instruction.compare("tya") == 0) {
            ins = INS_TYA;
            ticks += 2;
        }
        else if (instruction.compare("tsx") == 0) {
            ins = INS_TSX;
            ticks += 2;
        }
        else if (instruction.compare("txs") == 0) {
            ins = INS_TXS;
            ticks += 2;
        }

        else if (instruction.compare("pha") == 0) {
            ins = INS_PHA;
            ticks += 3;
        }
        else if (instruction.compare("pla") == 0) {
            ins = INS_PLA;
            ticks += 4;
        }

        else if (instruction.compare("inx") == 0) {
            ins = INS_INX;
            ticks += 2;
        }
        else if (instruction.compare("iny") == 0) {
            ins = INS_INY;
            ticks += 2;
        }
        else if (instruction.compare("dex") == 0) {
            ins = INS_DEX;
            ticks += 2;
        }
        else if (instruction.compare("dey") == 0) {
            ins = INS_DEY;
            ticks += 2;
        }

        else if (instruction.compare("nop") == 0) {
            ins = INS_NOP;
            ticks += 2;
        }

        else if (instruction.compare("rts") == 0) {
            ins = INS_RTS;
            ticks += 6;
        }

        else if (instruction.compare("sec") == 0) {
            ins = INS_SEC;
            ticks += 2;
        }
        else if (instruction.compare("sed") == 0) {
            ins = INS_SED;
            ticks += 2;
        }
        else if (instruction.compare("sei") == 0) {
            ins = INS_SEI;
            ticks += 2;
        }
        else if (instruction.compare("clc") == 0) {
            ins = INS_CLC;
            ticks += 2;
        }
        else if (instruction.compare("cld") == 0) {
            ins = INS_CLD;
            ticks += 2;
        }
        else if (instruction.compare("cli") == 0) {
            ins = INS_CLI;
            ticks += 2;
        }
        else if (instruction.compare("clv") == 0) {
            ins = INS_CLV;
            ticks += 2;
        }

        else if (instruction.compare("and") == 0) {
            ins = INS_AND_IM;
            ticks += 2;
        }
        else if (instruction.compare("andzp") == 0) {
            ins = INS_AND_ZP;
            ticks += 3;
        }
        else if (instruction.compare("andzpx") == 0) {
            ins = INS_AND_ZPX;
            ticks += 4;
        }

        else if (instruction.compare("dec") == 0) {
            ins = INS_DEC_ZP;
            ticks += 4;
        }
        else if (instruction.compare("decx") == 0) {
            ins = INS_DEC_ZPX;
            ticks += 5;
        }
        else if (instruction.compare("inc") == 0) {
            ins = INS_INC_ZP;
            ticks += 4;
        }
        else if (instruction.compare("incx") == 0) {
            ins = INS_INC_ZPX;
            ticks += 5;
        }

        else if (instruction.compare("asl") == 0) {
            ins = INS_ASL_ACC;
            ticks += 2;
        }
        else if (instruction.compare("aslzp") == 0) {
            ins = INS_ASL_ZP;
            ticks += 5;
        }
        else if (instruction.compare("aslzpx") == 0) {
            ins = INS_ASL_ZPX;
            ticks += 6;
        }

        else if (instruction.compare("lsr") == 0) {
            ins = INS_LSR_ACC;
            ticks += 2;
        }
        else if (instruction.compare("lsrzp") == 0) {
            ins = INS_LSR_ZP;
            ticks += 5;
        }
        else if (instruction.compare("lsrzpx") == 0) {
            ins = INS_LSR_ZPX;
            ticks += 6;
        }

        else if (instruction.compare("or") == 0) {
            ins = INS_ORA_IM;
            ticks += 2;
        }
        else if (instruction.compare("orzp") == 0) {
            ins = INS_ORA_ZP;
            ticks += 3;
        }
        else if (instruction.compare("orzpx") == 0) {
            ins = INS_ORA_ZPX;
            ticks += 4;
        }

        else if (instruction.compare("jmp") == 0) {
            ins = INS_JMP;
            ticks += 3;
        }
        else if (instruction.compare("jsr") == 0) {
            ins = INS_JSR;
            ticks += 4;
        }
        else {
            ins = INS_NOP;
            ticks += 2;
        }

        memory[counter] = ins;
        counter++;
    }

    void InterpretNumbers(Byte value, Word& counter, MEMORY& memory) {
        memory[counter] = value;
        counter++;
    }

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

            case INS_AND_IM: {
                Byte value = Fetch(ticks, memory);
                a = a & value;
                LDASetFlags();
            } break;

            case INS_AND_ZP: {
                Byte ZeroPageAddress = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPageAddress, memory);
                a = a & value;
                LDASetFlags();
            } break;

            case INS_AND_ZPX: {
                Byte ZeroPageAddress = Fetch(ticks, memory);
                ZeroPageAddress += x;
                ticks--;
                Byte value = Read(ticks, ZeroPageAddress, memory);
                a = a & value;
                LDASetFlags();
            } break;

            case INS_DEC_ZP: {
                Byte ZeroPage = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPage, memory);
                value--;
                memory.Write(value, ZeroPage, ticks);
                zero = (value == 0);
                negative = (value & 0b10000000) > 0;
            } break;

            case INS_DEC_ZPX: {
                Byte ZeroPage = Fetch(ticks, memory);
                ZeroPage += x;
                ticks--;
                Byte value = Read(ticks, ZeroPage, memory);
                value--;
                memory.Write(value, ZeroPage, ticks);
                zero = (value == 0);
                negative = (value & 0b10000000) > 0;
            } break;

            case INS_INC_ZP: {
                Byte ZeroPage = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPage, memory);
                value++;
                memory.Write(value, ZeroPage, ticks);
                zero = (value == 0);
                negative = (value & 0b10000000) > 0;
            } break;

            case INS_INC_ZPX: {
                Byte ZeroPage = Fetch(ticks, memory);
                ZeroPage += x;
                ticks--;
                Byte value = Read(ticks, ZeroPage, memory);
                value--;
                memory.Write(value, ZeroPage, ticks);
                zero = (value == 0);
                negative = (value & 0b10000000) > 0;
            } break;

            case INS_ASL_ACC: {
                a = a << 1;
                ticks--;
                LDASetFlags();
            } break;

            case INS_ASL_ZP: {
                Byte ZeroPage = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPage, memory);
                value = value << 1;
                ticks--;
                memory.Write(value, ZeroPage, ticks);
                carry = negative = (value & 0b10000000) > 0;
                zero = (value == 0);
            } break;

            case INS_ASL_ZPX: {
                Byte ZeroPage = Fetch(ticks, memory);
                ZeroPage += x;
                ticks--;
                Byte value = Read(ticks, ZeroPage, memory);
                value = value << 1;
                ticks--;
                memory.Write(value, ZeroPage, ticks);
                carry = negative = (value & 0b10000000) > 0;
                zero = (value == 0);
            } break;

            case INS_LSR_ACC: {
                a = a >> 1;
                ticks--;
                LDASetFlags();
            } break;

            case INS_LSR_ZP: {
                Byte ZeroPage = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPage, memory);
                value = value >> 1;
                ticks--;
                memory.Write(value, ZeroPage, ticks);
                carry = negative = (value & 0b10000000) > 0;
                zero = (value == 0);
            } break;

            case INS_LSR_ZPX: {
                Byte ZeroPage = Fetch(ticks, memory);
                ZeroPage += x;
                ticks--;
                Byte value = Read(ticks, ZeroPage, memory);
                value = value >> 1;
                ticks--;
                memory.Write(value, ZeroPage, ticks);
                carry = negative = (value & 0b10000000) > 0;
                zero = (value == 0);
            } break;

            case INS_ORA_IM: {
                Byte value = Fetch(ticks, memory);
                a |= value;
                LDASetFlags();
            } break;

            case INS_ORA_ZP: {
                Byte ZeroPage = Fetch(ticks, memory);
                Byte value = Read(ticks, ZeroPage, memory);
                a |= value;
                LDASetFlags();
            } break;

            case INS_ORA_ZPX: {
                Byte ZeroPage = Fetch(ticks, memory);
                ZeroPage += x;
                ticks--;
                Byte value = Read(ticks, ZeroPage, memory);
                a |= value;
                LDASetFlags();
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

    void replace(char v, char c, string& str) {
        size_t index = str.find_first_of(v);
        // str.replace(index, 1, c);
    }

    u32 splitByNewLine(string sentence, MEMORY& memory) {
        stringstream ss;
        string line;

        Word counter = program_counter;
        u32 ticks = 0;

        ss << sentence;

        int16_t found;
        string temp;
        while (!ss.eof()) {
            ss >> temp;

            if (counter < program_counter) {
                cout << "Program size is too big. Exit";
                exit(3);
            }

            if (istringstream(temp) >> found)
                InterpretNumbers(found, counter, memory);
            else {
                interpretInstruction(temp, counter, memory, ticks);
            }
        }
        return ticks;
    }

    string ReadFileInstructions(string FileName) {
        string TextOfInstructions;
        string Output;

        ifstream InstructionFile(FileName);

        while (getline(InstructionFile, TextOfInstructions)) {
            Output += "\n" + TextOfInstructions;
        }

        InstructionFile.close();

        return Output;
    }
};

int main() {
    MEMORY memory;
    CPU6502 cpu;
    cpu.Reset(memory);

    string result = cpu.ReadFileInstructions("./program.xndr");

    u32 ticks = cpu.splitByNewLine(result, memory);

    cpu.Execute(ticks, memory);

    if (cpu.program_counter < 0xFF00) {
        printf("Program counter overflow. Exit");
        return 1;
    }

    printf("a: %d\nx: %d\ny: %d", cpu.a, cpu.x, cpu.y);

    return 0;
};
