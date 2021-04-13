# 6502-Emulator

This is an emulator for a 6502 Processor coded in C++

The only way to run a program is to hard-code it in main.cpp and provide the correct ticks for the execution function.

Current hardcoded program logs this:

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
 - **TAX** - Transfer a to x
 - **TAY** - Transfer a to y
 - **TXA** - Transfer x to a
 - **TYA** - Transfer y to a
 - **TSX** - Transfer stackpointer to x
 - **TXS** - Transfer x to stackpointer
 - **PHA** - Push a to stack
 - **PLA** - Pull a from stack
 - **INX** - Increment x
 - **INY** - Increment y

I'm working on more functions
