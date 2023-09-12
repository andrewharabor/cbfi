# cbfi v1.0.0

## About

An interpreter written in C for the esoteric programming language [Brainf*ck](https://en.wikipedia.org/wiki/Brainfuck).

## Specifications

Currently, the interpreter implements a 32,768 byte memory tape with 8-bit cells, all initialized to 0. The values of the cells wrap around upon exceeding designated values as does the instruction pointer when it reaches beyond the allocated memory. Input and output take the form of ASCII characters, meaning the character encoding itself is stored in the memory cells. Unmatched will brackets result in a syntax error.

## Usage

To run a Brainf*ck program, compile cbfi.c and run the executable with the .bf file as a command-line argument. For Linux, this might look like the following:

``` bash
./cbfi [FILENAME.bf]
```
