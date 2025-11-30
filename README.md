This graph showcases the path we take to go from a source file containing code to actually running that code on a computer

 Source    Tokens      AST        IR        Optimized IR    Assembly    Binary
    │         │         │          │             │             │          │
    ▼         ▼         ▼          ▼             ▼             ▼          ▼
┌──────┐  ┌──────┐  ┌──────┐  ┌───────┐   ┌──────────┐   ┌────────┐  ┌───────┐
│ .c   │─►│Lexer │─►│Parser│─►│IR Gen │──►│Optimizer │──►│CodeGen │─►│Linker │
└──────┘  └──────┘  └──────┘  └───────┘   └──────────┘   └────────┘  └───────┘

Lexer - Breaks code into small tokens
        "int x = 5;" -> {INT, ID(x), ASSIGN, NUM(5), SEMI}
        
Parser -> Builds an Abstract Syntax Tree(AST) from tokens, capturing the code's structure
          "ASSIGN(x, 5)"

IR Generator - Converts the AST into Intermediate Representation, a simple and platform-independent instruction format
              "%1 = alloca i32
              store i32 5, i32* %1"

Optimizer - Applies transformation passes to improve the performance of the code, such as removing useless instructions

Code Generator -> Emits actual x86 instructions or instructions for any other platform.
