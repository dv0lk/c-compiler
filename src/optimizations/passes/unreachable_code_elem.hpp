#pragma once
#include "flow_graph/flow_graph.hpp"

namespace compiler {
    class UnreachableCode {
    public:
        using FlowGraphType = FlowGraph<ir::ir_instruction>;

        bool apply(FlowGraphType& flow_graph);

    private:
        bool holds_any_jump(const ir::ir_instruction& instruction);

        bool remove_unreachable_blocks(FlowGraphType& flow_graph);

        bool remove_redundant_jumps(FlowGraphType& flow_graph);

        bool remove_redundant_labels(FlowGraphType& flow_graph);
    };
}
