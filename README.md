# 6502-Emulator

This is an emulator for a 6502 Processor coded in C++

Currently the only way to let it execute some program is to hardcode it into the main.cpp with providing the right ticks to the execute function.

Current setup logs

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

I'm working on more functions
