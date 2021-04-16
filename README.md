# 6502-Emulator

This is an emulator for a 6502 Processor coded in C++

The only way to run a program is to hard-code it in main.cpp and provide the correct ticks for the execution function.

Current hardcoded program outputs this:

```
a register: 3
x register: 9
y register: 5
```

## Working functions

 - **LDA** - Load memory into A registry
 - **LDX** - Load memory into X registry
 - **LDY** - Load memory into Y registry
 - **STA** - Store the value from A registry into a memory address
 - **STX** - Store the value from X registry into a memory address
 - **STY** - Store the value from Y registry into a memory address
 - **JSR** - Jump to a subroutine
 - **TAX** - Transfer A to X
 - **TAY** - Transfer A to Y
 - **TXA** - Transfer X to A
 - **TYA** - Transfer Y to A
 - **TSX** - Transfer stackpointer to X
 - **TXS** - Transfer X to stackpointer
 - **PHA** - Push A to stack
 - **PLA** - Pull A from stack
 - **INX** - Increment X
 - **INY** - Increment Y

I'm working on more functions
