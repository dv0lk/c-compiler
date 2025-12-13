#pragma once
#include "codegen/code_generator.hpp"
#include "ir/generator.h"
#include "lexer/lexer.h"
#include "optimizations/optimizer.hpp"
#include "parser/ast.h"

namespace compiler {
    class Compiler {
    private:
        lexer::lexer lexer;
        ast::parser parser;
        ir::generator ir_generator;
        // Optimizer optimizer;
        // CodeGenerator code_generator;

    public:
        void compile(const std::string& source);
    };
}
