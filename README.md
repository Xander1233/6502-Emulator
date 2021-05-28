# 6502-Emulator

This is an emulator for a 6502 Processor coded in C++

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
 - **DEX** - Decrement X
 - **DEY** - Decrement Y
 - **NOP** - No operation
 - **RTS** - Return from subroutine
 - **SEC** - Set carry flag
 - **SED** - Set decimal mode
 - **SEI** - Set interrupt disable
 - **CLC** - Clear carry flag
 - **CLD** - Clear decimal mode
 - **CLI** - Clear interrupt disable
 - **CLV** - Clear overflow flag
 - **AND** - Logical AND
 - **DEC** - Decrement memory
 - **INC** - Increment memory
 - **ASL** - Arithmetic bit wise shift left
 - **LSR** - Logical shift right
 - **ORA** - Logical inclusive OR
 - **JMP** - Jump

### Operation codes as words

**To run a program you might use the words below in the separate file called program.txt in the same directory**

 - **ldaim** - Load Accumulator | Immediate
 - **ldazp** - Load Accumulator | Zero Page
 - **ldazpx** - Load Accumulator | Zero Page X
 - **ldxim** - Load X | Immediate
 - **ldxzp** - Load X | Zero Page
 - **ldxzpy** - Load X | Zero Page Y
 - **ldyim** - Load Y | Immediate
 - **ldyzp** - Load Y | Zero Page
 - **ldyzpx** - Load Y | Zero Page X
 - **sta** - Store Accumulator | Zero Page
 - **stax** - Store Accumulator | Zero Page X
 - **stx** - Store X | Zero Page
 - **stxy** - Store X | Zero Page Y
 - **sty** - Store Y | Zero Page
 - **styx** - Store Y | Zero Page X
 - **tax** - Transfer Accumulator into X
 - **tay** - Transfer Accumulator into Y
 - **txa** - Transfer X into Accumulator
 - **tya** - Transfer Y into Accumulator
 - **tsx** - Transfer stack pointer into X
 - **txs** - Transfer X into stack pointer
 - **pha** - Push Accumulator on stack
 - **pla** - Pull Accumulator from stack
 - **inx** - Increment X
 - **iny** - Increment Y
 - **dex** - Decrement X
 - **dey** - Decrement Y
 - **nop** - No operation
 - **rts** - Return from subroutine
 - **sec** - Set carry flag
 - **sed** - Set decimal flag
 - **sei** - Set interrupt flag
 - **clc** - Clear carry flag
 - **cld** - Clear decimal flag
 - **cli** - Clear interrupt flag
 - **clv** - Clear overflow flag
 - **and** - Logical AND | Immediate
 - **andzp** - Logical AND | Zero Page
 - **andzpx** - Logical AND | Zero Page X
 - **dec** - Decrement | Zero Page
 - **decx** - Decrement | Zero Page X
 - **inc** - Increment | Zero Page
 - **incx** - Increment | Zero Page X
 - **asl** - Arithmetic bit wise shift left | Immediate
 - **aslzp** - Arithmetic bit wise shift left | Zero Page
 - **aslzpx** - Arithmetic bit wise shift left | Zero Page X
 - **lsr** - Logical shift right | Immediate
 - **lsrzp** - Logical shift right | Zero Page
 - **lsrzpx** - Logical shift right | Zero Page X
 - **or** - Logical inclusive OR | Immediate
 - **orzp** - Logical inclusive OR | Zero Page
 - **orzpx** - Logical inclusive OR | Zero Page X
 - **jmp** - Jump | Absolute
 - **jsr** - Jump to subroutine

*Example*:
```
ldaim
25
ldxim
35
dex
```
=>
```
a: 25
x: 34
y: 0
```

**Addresses and numbers have to be in Base 10**
**Every operation, argument or address has to be on his own line**

*Every unknown operation results in a **nop**, except if it's a number/address*
