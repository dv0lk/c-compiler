#include "lexer/tokenizer.h"
#include "parser/ast.hpp"
#include "ir/ir_emitter.h"
#include "util/printer.h"
#include "util/files.h"
#include <filesystem>
#include <print>
#include "transforms/manager.hpp"
#include "transforms/transforms/constant_fold.hpp" //TODO should either create a single header for all transforms, or do somethign different
#include "transforms/transforms/copy_propagation.hpp"
#include "x86/x86_emitter.hpp"
#include "util/x86_printer.hpp"
#include "analysis/cfg.hpp"
namespace compiler {
    void startup(const std::string& source) {
        auto tokens = lexer::tokenizer::get_tokens(source);
        auto ast = ast::parser::get_ast(tokens);
        auto ir = ir::emitter::get_ir(ast);

        // auto x86 = x86::emitter::get_x86(ir);
        // auto cfg = CFG<x86::Instruction>::from_function(x86.functions().front());


        // auto cfg = CFG<x86::Instruction>::from_function(x86_instructions);
        // CFG<> cfg = CFG::get_cfg(ir.functions().front().blocks());
        // x86::util::print_instructions(x86_instructions);
        // std::println("");
        // std::println("{}", ir::printer::to_string(ir));
        //
        // TransformManager<ir::instruction> tm;
        // tm.register_transform<transforms::ConstantFolding<ir::instruction>>();
        // tm.register_transform<transforms::CopyPropagation<ir::instruction>>();
        // tm.run_on_program(ir);
        //
        // std::println("{}", ir::printer::to_string(ir));
    }
}


int main() {
    const auto project_path = std::filesystem::current_path().parent_path();
    const std::string path = (project_path / "tests" / "test.c").string();

    const auto file = files::read_file(path);
    if (!file.has_value())
        throw std::runtime_error("Failed to read file");

    const std::string source = {file->begin(), file->end()};

    compiler::startup(source);
}
