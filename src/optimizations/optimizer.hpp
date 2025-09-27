#pragma once
#include <vector>
#include "flow_graph/flow_graph.hpp"
#include "ir/ir.h"
#include "passes/constant_folding.hpp"
#include "passes/copy_propagation.hpp"

//TODO rework this probably
namespace compiler {
    class Optimizer {
    private:
        ConstantFolding folding;
        CopyPropagation copy_propagation;
        FlowGraph<ir::ir_instruction> graph;

    public:
        Optimizer() = default;

        std::vector<ir::ir_basic_block> optimize(const std::vector<ir::ir_basic_block>& blocks) {
            std::vector<ir::ir_basic_block> optimized_blocks;

            for (const auto& block : blocks) {
                bool changed;
                int limit = 0;
                do {
                    changed = false;
                    auto block_instructions = block.get_instructions();
                    graph.generate_flowgraph(block_instructions);

                    //todo bug with infinite loop, forced to do limit
                    changed |= folding.apply(block_instructions);
                    changed |= copy_propagation.apply(graph);
                    limit++;


                } while (changed && limit < 25);

                const auto optimized_instructions = graph.emit_instructions();
                ir::ir_basic_block optimized_block = block;
                optimized_block.instructions = optimized_instructions;
                optimized_blocks.push_back(optimized_block);
            }

            return optimized_blocks;
        }
    };
}
