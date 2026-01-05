#include "backend/analysis/cfg.hpp"
#include "backend/ir/emitter.hpp"
#include "backend/transforms/manager.hpp"
#include "backend/transforms/passes/passes.hpp"
#include "backend/x86/codegen.hpp"
#include "compiler/frontend/lexer/lexer.hpp"
#include "compiler/frontend/parser/parser.hpp"
#include "runtime.hpp"
#include "util/config.hpp"
#include "util/format/format.hpp"
#include "x86/reg_alloc.hpp"
#include <print>

namespace compiler {
    void startup(const Config& config) {
        auto tokens = lexer::Tokenizer::get_tokens(config.input_file);
        auto ast = ast::Parser::get_ast(tokens);
        auto ir = ir::Emitter::get_ir(ast);

        if (config.print_ir_no_opt) {
            std::println("{}", ir);
        }

        TransformManager<ir::Instruction> ir_transforms;
        ir_transforms.register_transform<transforms::ConstantFolding<ir::Instruction>>();
        ir_transforms.run_on_program(ir);

        if (config.print_ir) {
            std::println("{}", ir);
        }

        auto x86 = x86::Emitter::get_x86(ir);

        if (config.print_x86_no_opt) {
            std::println("{}", x86);
        }

        x86::RegisterAllocator allocator;
        allocator.run_on_program(x86);

        TransformManager<x86::Instruction> x86_transforms;
        x86_transforms.register_transform<transforms::Peephole>();
        x86_transforms.run_on_program(x86);

        if (config.print_x86) {
            std::println("{}", x86);
        }

        std::ofstream file("output.s");
        std::println(file, "{}", x86);
        file.close();
        write_runtime("runtime.c");

        if (config.link) {
            std::system("clang output.s runtime.c -o result.exe");
        }
    }
} // namespace compiler

int main(int argc, char* argv[]) {
    Config config;
    config.init(argc, argv);
    compiler::startup(config);
}
