# cbfi

## About

An interpreter written in C for the esoteric programming language [Brainf*ck](https://en.wikipedia.org/wiki/Brainfuck).

## Specifications

- 32,768 cell memory tape with 8-bit unsigned values, all initialized to 0.
- The values of the cells wrap around upon exceeding the maximum or minimum value.
- The instruction pointer wraps around when it reaches past the allocated memory tape.
- Input and output use ASCII character encoding.
- Unmatched brackets result in a syntax error and program execution is halted immediately.

## Usage

Compile cbfi.c and run the executable accordingly.

``` text
USAGE: cbfi [-h] file.bf
    -h: print a short description about the interpreter
    file.bf: the Brainf*ck program to interpret and execute
```
