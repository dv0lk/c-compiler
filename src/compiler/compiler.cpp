#include "compiler.hpp"

#include "ir/printer.h"
#include "flow_graph/cfg.hpp"
namespace compiler {
    void Compiler::compile(const std::string& source) {
        //TODO fix const here
        auto tokens = lexer.parse_tokens(source);
        const auto ast = parser.parse_ast(tokens);
        const auto ir = ir_generator.generate(ast);
        std::println("{}", ir::printer::printer::to_string(ir));
        cfg::cfg cfg;
        cfg.generate_cfg(ir);

        std::println("ir:");
        // std::println("{}", ir::printer::ir_printer::to_string(ir));
        // code_generator.generate(ir.at(0).get_instructions());
        // auto optimized_ir = optimizer.optimize(ir);
        // std::println("optimized: ");
        // std::println("{}", ir::printer::ir_printer::to_string(optimized_ir));
    }
}





















