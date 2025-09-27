#include "copy_propagation.hpp"

#include <algorithm>
#include <deque>
#include <ranges>
#include <stdexcept>

namespace compiler {
    bool CopyPropagation::apply(FlowGraphType& graph) {
        bool changed = false;
        find_reaching_copies(graph);

        for (auto& node : graph.nodes) {
            std::vector<ir::ir_instruction> instructions_to_keep;

            for (size_t i = 0; i < node.instructions.size(); ++i) {
                const auto& instruction = node.instructions[i];
                auto reaching_copies = get_instruction_annotations(node.id, i);

                auto instr_copy = instruction;
                const auto new_instruction = std::visit([this, &reaching_copies](auto& instr) {
                    return this->rewrite_instruction(instr, reaching_copies);
                }, instr_copy);
                //TODO bug, when we don't rewrite the instruction, we still receive the value, so this will infinite loop
                if (new_instruction.has_value()) {
                    instructions_to_keep.emplace_back(new_instruction.value());
                    changed = true;
                }
            }
            node.instructions = std::move(instructions_to_keep);
        }
        return changed;
    }

    void CopyPropagation::find_reaching_copies(FlowGraphType& graph) {
        const auto all_copies = find_all_copy_instr(graph);
        std::deque<int> worklist;

        for (const auto& node : graph.nodes) {
            if (node.id == ENTRY || node.id == EXIT)
                continue;

            worklist.push_back(node.id);
            annotate_block(node, all_copies);
        }

        while (!worklist.empty()) {
            const auto block_id = worklist.front();
            worklist.pop_front();

            auto block = graph.get_node_from_id(block_id);
            auto old_annotation = get_block_annotation(block.id);
            auto incoming_copies = meet(block, all_copies);
            transfer(block, incoming_copies);

            if (old_annotation != get_block_annotation(block_id)) {
                for (auto successor_id : block.successors) {
                    if (successor_id == EXIT)
                        continue;

                    if (successor_id == ENTRY)
                        throw std::runtime_error("Malformed control graph");

                    if (!std::ranges::contains(worklist, successor_id))
                        worklist.push_back(successor_id);
                }
            }
        }
    }

    //todo simplify this with ranges probably
    std::vector<ir::ir_copy> CopyPropagation::find_all_copy_instr(const FlowGraphType& graph) {
        std::vector<ir::ir_copy> result;
        for (const auto& node : graph.nodes) {
            for (const auto& instr : node.instructions) {
                if (std::holds_alternative<ir::ir_copy>(instr)) {
                    auto copy = std::get<ir::ir_copy>(instr);
                    result.emplace_back(copy);
                }
            }
        }

        return result;
    }


    std::vector<ir::ir_copy> CopyPropagation::meet(const NodeType& node, const std::vector<ir::ir_copy>& all_copies) {
        std::vector<ir::ir_copy> incoming_copies = all_copies;

        for (const auto predecessor_id : node.predecessors) {
            if (predecessor_id == ENTRY)
                return {};

            if (predecessor_id == EXIT)
                throw std::runtime_error("Malformed control flow");

            auto predecessor_copies = get_block_annotation(predecessor_id);

            auto intersection = incoming_copies | std::views::filter([&](const ir::ir_copy& copy) -> bool {
                return contains_instruction(predecessor_copies, copy);
            }) | std::ranges::to<std::vector<ir::ir_copy> >();

            incoming_copies = std::move(intersection);
        }
        return incoming_copies;
    }

    bool CopyPropagation::contains_instruction(const std::vector<ir::ir_copy>& copy_instructions, const ir::ir_copy& copy_instruction) {
        return std::ranges::any_of(copy_instructions, [&copy_instruction](const ir::ir_copy& instr) {
            return instr.source == copy_instruction.source && instr.destination == copy_instruction.destination;
        });
    }

    void CopyPropagation::annotate_instruction(int block_id, size_t instr_index,
                                               const std::vector<ir::ir_copy>& reaching_copies) {
        annotated_instructions[{block_id, instr_index}] = reaching_copies;
    }

    std::vector<ir::ir_copy> CopyPropagation::get_instruction_annotations(int block_id, size_t instr_index) {
        const auto it = annotated_instructions.find({block_id, instr_index});
        if (it != annotated_instructions.end())
            return it->second;

        throw std::runtime_error("get_instruction_annotation on unknown instruction");
    }

    void CopyPropagation::annotate_block(const NodeType& node, const std::vector<ir::ir_copy>& reaching_copies) {
        annotated_blocks[node.id] = reaching_copies;
    }

    std::vector<ir::ir_copy> CopyPropagation::get_block_annotation(int node_id) {
        const auto it = annotated_blocks.find(node_id);

        if (it != annotated_blocks.end())
            return it->second;

        throw std::runtime_error("Trying to get not annotated block");
    }

    void CopyPropagation::kill_copies(std::vector<ir::ir_copy>& reaching_copies, const ir::ir_value& value) {
        std::erase_if(reaching_copies, [&](const ir::ir_copy& copy) -> bool {
            return copy.source == value || copy.destination == value;
        });
    }

    //Computes block reaching copies
    void CopyPropagation::transfer(NodeType& node, std::vector<ir::ir_copy>& initial_reaching_copies) {
        auto current_reaching_copies = initial_reaching_copies;

        for (size_t i = 0; i < node.instructions.size(); ++i) {
            const auto& instruction = node.instructions[i];
            annotate_instruction(node.id, i, current_reaching_copies);

            if (std::holds_alternative<ir::ir_copy>(instruction)) {
                const auto copy = std::get<ir::ir_copy>(instruction);

                if (contains_instruction(current_reaching_copies, copy))
                    continue;

                kill_copies(current_reaching_copies, copy.destination);
                current_reaching_copies.push_back(copy);
            }

            if (std::holds_alternative<ir::ir_unary>(instruction)) {
                const auto unary = std::get<ir::ir_unary>(instruction);
                kill_copies(current_reaching_copies, unary.result);
            }

            if (std::holds_alternative<ir::ir_binary>(instruction)) {
                const auto binary = std::get<ir::ir_binary>(instruction);
                kill_copies(current_reaching_copies, binary.result);
            }

            if (std::holds_alternative<ir::ir_call>(instruction)) {
                const auto call = std::get<ir::ir_call>(instruction);
                kill_copies(current_reaching_copies, call.destination);
            }

        }

        annotate_block(node, current_reaching_copies);
    }

    ir::ir_value CopyPropagation::replace_operand(ir::ir_value& operand, std::vector<ir::ir_copy>& reaching_copies) {
        if (operand.is_constant())
            return operand;

        for (const auto& [destination, source] : reaching_copies) {
            if (destination == operand)
                return source;
        }

        return operand;
    }


    std::optional<ir::ir_instruction> CopyPropagation::rewrite_instruction(ir::ir_copy& copy, std::vector<ir::ir_copy>& reaching_copies) {
        for (const auto& reaching_copy : reaching_copies) {
            //delete
            bool tmp = reaching_copy.source == copy.destination && reaching_copy.destination == copy.source;
            if (reaching_copy == copy || tmp)
                return {};
        }

        const auto new_source = replace_operand(copy.source, reaching_copies);
        return ir::ir_copy{copy.destination, new_source};
    }

    std::optional<ir::ir_instruction> CopyPropagation::rewrite_instruction(ir::ir_unary& unary, std::vector<ir::ir_copy>& reaching_copies) {
        const auto new_source = replace_operand(unary.value, reaching_copies);
        return ir::ir_unary{unary.op, new_source, unary.result};
    }

    std::optional<ir::ir_instruction> CopyPropagation::rewrite_instruction(ir::ir_binary& binary, std::vector<ir::ir_copy>& reaching_copies) {
        const auto new_source1 = replace_operand(binary.left, reaching_copies);
        const auto new_source2 = replace_operand(binary.right, reaching_copies);
        return ir::ir_binary{binary.op, new_source1, new_source2, binary.result};
    }

    std::optional<ir::ir_instruction> CopyPropagation::rewrite_instruction(ir::ir_call& call, std::vector<ir::ir_copy>& reaching_copies) {
        std::vector<ir::ir_value> new_args;
        for (auto& arg : call.arguments) {
            new_args.push_back(replace_operand(arg,reaching_copies));
        }
        return ir::ir_call{call.function_name, new_args, call.destination};
    }
}
