# Small C compiler
Small toy C compiler with an LLVM like design, written in C++23. Implements a lexer, AST parser, IR emitter, x86 emitter, register allocation, and optimization passes.
## Build 
```commandline
cmake -B build
cmake --build build config Release
```
## Usage
```commandline
Usage: compiler [--help] [--ast] [--ir] [--ir-no-opt] [--x86] [--x86-no-opt] [--output VAR] input

A simple C compiler

Positional arguments:
  input         input source file (.c) [required]

Optional arguments:
  -h, --help    shows help message and exits
  --ast         print the AST
  --ir          print IR after optimizations
  --ir-no-opt   print IR before optimizations
  --x86         print x86 assembly after optimizations
  --x86-no-opt  print x86 assembly before optimizations
  -o, --output  output file path [default: "output.asm"]

Example: compiler input.c --ir --x86
```
