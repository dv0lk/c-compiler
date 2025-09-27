#include <algorithm>
#include <ranges>
#include "unreachable_code_elem.hpp"
#include "flow_graph/flow_graph.hpp"

namespace compiler {
    bool UnreachableCode::apply(FlowGraphType& flow_graph) {
        bool changed = false;

        changed |= remove_unreachable_blocks(flow_graph);
        changed |= remove_redundant_jumps(flow_graph);
        changed |= remove_redundant_labels(flow_graph);

        return changed;
    }

    bool UnreachableCode::holds_any_jump(const ir::ir_instruction& instruction) {
        return std::holds_alternative<ir::ir_jump>(instruction)
               || std::holds_alternative<ir::ir_jump_if_zero>(instruction)
               || std::holds_alternative<ir::ir_jump_if_not_zero>(instruction);
    }

    bool UnreachableCode::remove_unreachable_blocks(FlowGraphType& flow_graph) {
        bool changed = false;

        for (auto& node : flow_graph.nodes) {
            if (node.id == ENTRY || node.id == EXIT)
                continue;

            if (node.predecessors.empty()) {
                flow_graph.remove_node(node);
                changed = true;
            }
        }

        return changed;
    }

    bool UnreachableCode::remove_redundant_jumps(FlowGraphType& flow_graph) {
        bool changed = false;

        for (const auto& [index, node] : std::views::enumerate(flow_graph.nodes)) {
            if (node.id == EXIT)
                break;

            if (node.instructions.empty())
                continue;

            auto last_instruction = node.instructions.back();

            if (holds_any_jump(last_instruction)) {
                auto default_successor = flow_graph.nodes[index + 1];

                bool has_non_default_successor = std::ranges::any_of(node.successors, [&](auto successor_id) -> bool {
                                                                         return successor_id != default_successor.id;
                                                                     }
                    );

                if (!has_non_default_successor) {
                    node.instructions.pop_back();
                    changed = true;
                }
            }
        }

        return changed;
    }


    bool UnreachableCode::remove_redundant_labels(FlowGraphType& flow_graph) {
        bool changed = false;
        for (const auto& [index, node] : std::views::enumerate(flow_graph.nodes)) {
            if (node.id == EXIT)
                break;

            if (node.instructions.empty())
                continue;

            auto first_instruction = node.instructions.front();

            if (std::holds_alternative<ir::ir_label>(first_instruction)) {
                auto predecessors = flow_graph.nodes[index - 1];

                if (node.predecessors.size() == 1 && node.predecessors[0] == predecessors.id) {
                    node.instructions.erase(node.instructions.begin());
                    changed = true;
                }
            }
        }
        return changed;
    }
}
