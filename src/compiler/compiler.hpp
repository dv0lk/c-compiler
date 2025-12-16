#pragma once
#include "ir/ir_emitter.h"
#include "lexer/lexer.h"
#include "parser/ast.h"

namespace compiler {
    class Compiler {
    private:
        //TODO make all of these static
        lexer::lexer lexer;
        ast::parser parser;
        ir::emitter ir_generator;
        // Optimizer optimizer;
        // CodeGenerator code_generator;

    public:
        void compile(const std::string& source);
    };
}
