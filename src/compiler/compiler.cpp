#include "compiler.hpp"

#include "ir/ir_printer.h"

namespace compiler {
    void Compiler::compile(const std::string& source) {
        const auto tokens = lexer.parse_tokens(source);

        const auto ast = parser.parse_ast(tokens);

        auto ir = ir_generator.generate(ast);

        const auto optimized_ir = optimizer.optimize(ir);
        std::println("optimized: ");
        std::println("{}", ir::printer::ir_printer::to_string(optimized_ir));
        // code_generator.generate(optimized_ir);
    }
}
