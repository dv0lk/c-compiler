#pragma once
#include "codegen/code_generator.hpp"
#include "ir/ir_generator.h"
#include "lexer/lexer.h"
#include "optimizations/optimizer.hpp"
#include "parser/parser.h"

namespace compiler {
    class Compiler {
    private:
        lexer::lexer lexer;
        parser::parser parser;
        ir::ir_generator ir_generator;
        Optimizer optimizer;
        CodeGenerator code_generator;

    public:
        void compile(const std::string& source);
    };
}
