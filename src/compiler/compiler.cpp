#include "compiler.hpp"

#include "ir/printer.h"

namespace compiler {
    void Compiler::compile(const std::string& source) {
        auto tokens = lexer.parse_tokens(source);
        const auto ast = parser.parse_ast(tokens);

        const auto ir = ir_generator.generate(ast);
        std::println("{}", ir::printer::printer::to_string(ir));
        // std::println("ir:");
        // std::println("{}", ir::printer::ir_printer::to_string(ir));
        // code_generator.generate(ir.at(0).get_instructions());
        // auto optimized_ir = optimizer.optimize(ir);
        // std::println("optimized: ");
        // std::println("{}", ir::printer::ir_printer::to_string(optimized_ir));
    }
}





















