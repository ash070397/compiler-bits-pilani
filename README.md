# Compiler Construction

## Overview

The following project was completed in partial fulfilment of the undergraduate course at BITS Pilani: [Compiler Construction](https://www.bits-pilani.ac.in/Uploads/Pilani_Upload/Dept_Computer_Science_and_Information_Systems/Handouts/II_SEM_2016_2017/CS_F363.pdf) (_CS F363_). 
We build a compiler for a custom language, ERPLAG (similar to C), whose specifications are outlined [here](./Language-specifications.pdf). The compiler has been implemented in C with the following components

- Lexer
- Parser
- Abstract Syntax Tree
- Symmbol Table
- Type checker and Semanntic Analysis
- Code Generation for NASM

## How to run?

- Run `make` from within the [Stage2 directory](https://github.com/ash070397/compiler/tree/master/Stage2), this will generate executables for the aforementioned stages
- nasm -f elf32 code.asm -o code.o
- gcc -m32 code.o -o code.out
- ./code.out

For a detailed understanding of the components refer to our documentation [here](https://github.com/ash070397/compiler/blob/master/Stage2/Compiler.pdf)

## Dependencies

You should have the following dependencies installed:
- NASM >= 2.0.1.
- gcc >= 5.0.0
