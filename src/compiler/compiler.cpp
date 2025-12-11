#include "compiler.hpp"

#include "ir/ir_printer.h"

namespace compiler {
    void Compiler::compile(const std::string& source) {
        auto tokens = lexer.parse_tokens(source);
        const auto ast = parser.parse_ast(tokens);

        auto ir = ir_generator.generate(ast);
        std::println("ir:");
        // std::println("{}", ir::printer::ir_printer::to_string(ir));
        // code_generator.generate(ir.at(0).get_instructions());
        // auto optimized_ir = optimizer.optimize(ir);
        // std::println("optimized: ");
        // std::println("{}", ir::printer::ir_printer::to_string(optimized_ir));
    }
}





















