#include "compiler.hpp"


#include "analysis/CFG.hpp"
#include "transforms/manager.hpp"
#include "transforms/transforms/constant_fold.hpp"
#include "transforms/transforms/copy_propagation.hpp"
namespace compiler {
    void Compiler::compile(const std::string& source) {
        //TODO fix const here
        auto tokens = lexer.parse_tokens(source);
        const auto ast = parser.parse_ast(tokens);
        // auto ir_program = ir::emitter::get_ir(ast);

        // TransformManager<std::vector<ir::basic_block_t>> tm;
        // tm.register_transform<transforms::ConstantFolding>();
        // tm.register_transform<transforms::CopyPropagation>();
        // std::println("{}", ir::printer::printer::to_string(ir));
        // std::println("----------------------------------");

        // //TODO move this shit out of here
        // bool changed = true;
        // while (changed) {
        //     changed = false;
        //     for (auto& block : ir) {
        //         changed |= tm.run_all(ir);
        //     }
        // }
        //

    }
}





















