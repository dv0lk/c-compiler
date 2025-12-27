#include "compiler/frontend/lexer/lexer.hpp"
#include "compiler/frontend/parser.hpp"
#include "backend/ir/emitter.hpp"
#include <print>
#include "backend/transforms/manager.hpp"
#include "backend/transforms/transforms/constant_fold.hpp" //TODO should either create a single header for all transforms, or do somethign different
#include "backend/transforms/transforms/copy_propagation.hpp"
#include "backend/transforms/transforms/dead_code_elem.hpp"
#include "backend/x86/codegen.hpp"
#include "backend/analysis/cfg.hpp"
#include "config/config.hpp"
#include "util/format/format.hpp"
#include "x86/reg_alloc.hpp"

namespace compiler {
    void startup(const Config& config) {
        auto tokens = lexer::tokenizer::get_tokens(config.input_file);
        auto ast = ast::Parser::get_ast(tokens);
        auto ir = ir::Emitter::get_ir(ast);

        TransformManager<ir::Instruction> tm;
        tm.register_transform<transforms::ConstantFolding<ir::Instruction>>();
        tm.register_transform<transforms::DeadCodeElim<ir::Instruction>>();
        tm.run_on_program(ir);
        std::println("{}", ir);


        auto x86 = x86::emitter::get_x86(ir);
        // std::println("{}", x86);
        x86::RegisterAllocator allocator;
        allocator.run_on_program(x86);
        std::println("{}", x86);
        // auto cfg = CFG<x86::Instruction>::from_function(x86.functions().front());
        // auto cfg = CFG<x86::Instruction>::from_function(x86_instructions);
        // CFG<> cfg = CFG::get_cfg(ir.functions().front().blocks());
        // x86::util::print_instructions(x86_instructions);
        // std::println("");
        // std::println("{}", ir::printer::to_string(ir));
        // TransformManager<ir::instruction> tm;
        // tm.register_transform<transforms::ConstantFolding<ir::instruction>>();
        // tm.register_transform<transforms::CopyPropagation<ir::instruction>>();
        // tm.run_on_program(ir);
        // std::println("{}", ir::printer::to_string(ir));
    }
}


int main(int argc, char* argv[]) {
    Config config;
    config.init(argc, argv);
    compiler::startup(config);


    // return 0;
    // const auto project_path = std::filesystem::current_path().parent_path();
    // // const std::string path = (project_path / "tests" / "test.c").string();
    // const std::string path = "C:/Users/123/CLionProjects/compiler-rewrite/tests/test.c";
    // const auto file = files::read_file(path);
    // if (!file.has_value())
    //     throw std::runtime_error("Failed to read file");
    //
    // const std::string source = {file->begin(), file->end()};

}
