#include "backend/analysis/cfg.hpp"
#include "backend/ir/emitter.hpp"
#include "backend/transforms/manager.hpp"
#include "backend/transforms/types/types.hpp"
#include "backend/x86/codegen.hpp"
#include "compiler/frontend/lexer/lexer.hpp"
#include "compiler/frontend/parser.hpp"
#include "config/config.hpp"
#include "util/format/format.hpp"
#include "x86/reg_alloc.hpp"
#include <print>

namespace compiler {
    void startup(const Config& config) {
        auto tokens = lexer::Tokenizer::get_tokens(config.input_file);
        auto ast = ast::Parser::get_ast(tokens);
        auto ir = ir::Emitter::get_ir(ast);

        TransformManager<ir::Instruction> tm;
        tm.register_transform<transforms::ConstantFolding<ir::Instruction>>();
        tm.run_on_program(ir);
        std::println("{}", ir);

        auto x86 = x86::Emitter::get_x86(ir);
        x86::RegisterAllocator allocator;
        allocator.run_on_program(x86);
        std::println("{}", x86);
    }
} // namespace compiler

int main(int argc, char* argv[]) {
    Config config;
    config.init(argc, argv);
    compiler::startup(config);
}
