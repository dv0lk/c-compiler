# Small C compiler
Small toy C compiler with an LLVM like design, written in C++23. Implements a lexer, AST parser, IR emitter, x86 emitter, register allocation, and optimization passes.
## Build 
```commandline
git clone --recurse-submodules https://github.com/pind0s/compiler-rewrite.git
cd compiler-rewrite
cmake -B build
cmake --build build --config Release
```
## Usage
```commandline
Usage: compiler [--help] [--ir] [--ir-no-opt] [--x86] [--x86-no-opt] [--link] [--output VAR] input

A simple C compiler

Positional arguments:
  input         input source file (.c) [required]

Optional arguments:
  -h, --help    shows help message and exits
  --ir          print IR after optimizations
  --ir-no-opt   print IR before optimizations
  --x86         print x86 assembly after optimizations
  --x86-no-opt  print x86 assembly before optimizations
  -l, --link    link and produce executable
  -o, --output  output file path [default: "output.asm"]

Example: compiler input.c --ir --x86 --link
```

## Limitations 
1. Windows x64 is the only supported platform.
2. This project relies on `Clang` for the final linking stage. Ensure `clang` is available in your system PATH.
3.  Only the `int` type is currently supported. Floating-point numbers, pointers, and user-defined types are not implemented.
4.  As an educational project, the compiler contains bugs and edge-case failures. Optimizations are currently limited to basic constant folding and peephole passes.
<details>
<summary><strong>Example</strong></summary>
example.c:
  
```c
int print_int(int n);
int pause();

int fib(int n) {
    if (n <= 1) {
        return n;
    }

    return fib(n - 1) + fib(n - 2);
}

int main() {
    print_int(fib(15));
    pause();
    return 0;
}
```
runtime.c: 
```c
#include <stdio.h>
void print_int(int n) { 
	printf("%d\n", n); 
}

void pause() {
    printf("Press Enter to exit...");
    getchar();
}
```

output.s:

```asm
.intel_syntax noprefix
.extern pause
.extern print_int
.globl main

.text

fib:
    push rbp
    mov rbp, rsp
    push rbx
    push rsi
    push rdi
    sub rsp, 40
    mov rdi, rcx
    cmp rdi, 1
    mov rbx, 0
    setle bl
    cmp rbx, 0
    je if_end_1
    if_then_0:
    mov rax, rdi
    add rsp, 40
    pop rdi
    pop rsi
    pop rbx
    pop rbp
    ret
    if_end_1:
    mov rbx, rdi
    sub rbx, 1
    mov rcx, rbx
    call fib
    mov rsi, rax
    mov rbx, rdi
    sub rbx, 2
    mov rcx, rbx
    call fib
    mov rdi, rax
    mov rbx, rsi
    add rbx, rdi
    mov rax, rbx
    add rsp, 40
    pop rdi
    pop rsi
    pop rbx
    pop rbp
    ret

main:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 40
    mov rcx, 15
    call fib
    mov rbx, rax
    mov rcx, rbx
    call print_int
    mov rbx, rax
    call pause
    mov rbx, rax
    mov rax, 0
    add rsp, 40
    pop rbx
    pop rbp
    ret
```
</details>
