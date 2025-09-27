#pragma once
#include <concepts>
#include <vector>
#include "codegen/x86_instructions.hpp"
#include "ir/ir.h"

namespace compiler {
    enum node_type {
        EXIT = -1,
        ENTRY = 0,
    };

    template <typename T>
    concept InstructionType = std::same_as<T, ir::ir_instruction> || std::same_as<T, x86::instruction>;

    template <InstructionType T>
    struct Node {
        int id;
        std::vector<T> instructions;
        std::vector<int> predecessors;
        std::vector<int> successors;

        explicit Node(const int id)
            : id(id) {}

        Node(const int id, const std::vector<T>& instructions)
            : id(id),
              instructions(instructions) {}

        friend bool operator==(const Node& lhs, const Node& rhs) {
            return lhs.id == rhs.id;
        }

    };

    // template<>
    // struct Node<x86::instruction> {
    //     x86::Operand id;
    //     double spill_cost = 0.0;
    //     std::vector<x86::Operand> neighbors;
    //     std::optional<int> color = {};
    //     bool pruned = false;
    //
    //     explicit Node(const x86::Operand &id)
    //         : id(id) {
    //     }
    //
    //     void add_neighbor(const x86::Operand& operand) {
    //         neighbors.emplace_back(operand);
    //     }
    // };


    template <InstructionType T>
    struct InstructionAdapter;

    template <>
    struct InstructionAdapter<ir::ir_instruction> {
        using Instr = ir::ir_instruction;

        static bool is_label(const Instr& instruction) {
            return std::holds_alternative<ir::ir_label>(instruction);
        }

        static std::string label_name(const Instr& instruction) {
            if (const auto label = std::get_if<ir::ir_label>(&instruction)) {
                return label->name;
            }
            throw std::runtime_error("Instruction is not label");
        }

        static bool is_conditional_jump(const Instr& instruction) {
            return std::holds_alternative<ir::ir_jump_if_zero>(instruction)
                   || std::holds_alternative<ir::ir_jump_if_not_zero>(instruction);
        }

        static bool is_unconditional_jump(const Instr& instruction) {
            return std::holds_alternative<ir::ir_jump>(instruction);
        }

        static std::string jump_label(const Instr& instruction) {
            if (std::holds_alternative<ir::ir_jump>(instruction))
                return std::get<ir::ir_jump>(instruction).label.name;

            if (std::holds_alternative<ir::ir_jump_if_zero>(instruction))
                return std::get<ir::ir_jump_if_zero>(instruction).label.name;

            if (std::holds_alternative<ir::ir_jump_if_not_zero>(instruction))
                return std::get<ir::ir_jump_if_not_zero>(instruction).label.name;

            throw std::runtime_error("Instruction is not jump");
        }

        static bool is_block_starter(const Instr& instruction) {
            return std::holds_alternative<ir::ir_jump>(instruction)
                   || std::holds_alternative<ir::ir_jump_if_zero>(instruction)
                   || std::holds_alternative<ir::ir_return>(instruction);
        }

        static bool is_block_terminator(const Instr& instruction) {
            return is_label(instruction);
        }

        static bool is_ret(const Instr& instruction) {
            return std::holds_alternative<ir::ir_return>(instruction);
        }

    };

    template <>
    struct InstructionAdapter<x86::instruction> {
        using Instr = x86::instruction;

        static bool is_label(const Instr& instruction) {
            return std::holds_alternative<x86::label>(instruction);
        }

        static bool is_conditional_jump(const Instr& instruction) {
            return std::holds_alternative<x86::jmp_cc>(instruction);
        }

        static bool is_unconditional_jump(const Instr& instruction) {
            return std::holds_alternative<x86::jmp>(instruction);
        }

        static std::string jump_label(const Instr& instruction) {
            if (const auto jmp = std::get_if<x86::jmp>(&instruction))
                return jmp->target.name;

            if (const auto jmp_cc = std::get_if<x86::jmp_cc>(&instruction))
                return jmp_cc->target.name;

            throw std::runtime_error("Instruction is not jump");
        }

        static std::string label_name(const Instr& instruction) {
            if (const auto label = std::get_if<x86::label>(&instruction)) {
                return label->name.name;
            }
            throw std::runtime_error("Instruction is not label");
        }

        static bool is_block_starter(const Instr& instruction) {
            return std::holds_alternative<x86::jmp>(instruction)
                   || std::holds_alternative<x86::jmp_cc>(instruction)
                   || std::holds_alternative<x86::ret>(instruction);
        }

        static bool is_block_terminator(const Instr& instruction) {
            return is_label(instruction);
        }

        static bool is_ret(const Instr& instruction) {
            return std::holds_alternative<x86::ret>(instruction);
        }
    };

    template <InstructionType T>
    class FlowGraph {
    public:
        using NodeType = Node<T>;
        std::vector<NodeType> nodes;

        void generate_flowgraph(const std::vector<T>& instructions) {
            auto basic_blocks = partition_to_bb(instructions);
            nodes.clear();

            nodes.emplace_back(ENTRY);

            for (auto& bb : basic_blocks)
                nodes.emplace_back(++id_counter, bb);

            nodes.emplace_back(EXIT);
            build_label_cache();
            add_all_edges();
        }

        [[nodiscard]] NodeType& get_node_from_id(int node_id) {
            for (auto& node : nodes) {
                if (node.id == node_id)
                    return node;
            }
            throw std::runtime_error("Node not found with id: " + std::to_string(node_id));
        }

        void remove_edge(const NodeType& node) {
            for (const auto successor : node.successors) {
                auto n = get_node_from_id(successor);
                std::erase(n.predecessors, node.id);
            }

            for (const auto predecessor : node.predecessors) {
                auto n = get_node_from_id(predecessor);
                std::erase(n.successors, node.id);
            }
        }

        void add_node(const NodeType& node) {
            nodes.emplace_back(node);
        }

        void remove_node(const NodeType& node) {
            remove_edge(node);
            std::erase(nodes, node);
        }

        void remove_node(const int id) {
            const auto node = get_node_from_id(id);
            remove_node(node);
        }


        [[nodiscard]] std::vector<T> emit_instructions() const {
            std::vector<T> instructions;
            for (const auto& node : nodes) {
                for (const auto& instr : node.instructions) {
                    instructions.emplace_back(instr);
                }
            }
            return instructions;
        }

    private:
        using adapter = InstructionAdapter<T>;
        int id_counter = 0;
        std::unordered_map<std::string, int> labels_to_block_id;

        void build_label_cache() {
            for (const auto& node : nodes) {
                if (!node.instructions.empty()) {
                    const auto instruction = node.instructions.front();
                    if (adapter::is_label(instruction)) {
                        auto name = adapter::label_name(instruction);
                        labels_to_block_id[name] = node.id;
                    }
                }
            }
        }

        int label_to_block_id(const std::string& label) {
            if (labels_to_block_id.contains(label)) {
                return labels_to_block_id[label];
            }
            throw std::runtime_error("Label not found: " + label);
        }

        std::vector<std::vector<T> > partition_to_bb(const std::vector<T>& instructions) {
            std::vector<std::vector<T> > finished_blocks;
            std::vector<T> current_block;

            for (const auto& instruction : instructions) {
                if (adapter::is_block_starter(instruction)) {
                    if (!current_block.empty())
                        finished_blocks.emplace_back(current_block);

                    current_block.clear();
                    current_block.push_back(instruction);
                    continue;
                }

                if (adapter::is_block_terminator(instruction)) {
                    current_block.emplace_back(instruction);
                    finished_blocks.emplace_back(current_block);
                    current_block.clear();
                    continue;
                }

                current_block.emplace_back(instruction);
            }

            if (!current_block.empty())
                finished_blocks.emplace_back(current_block);

            return finished_blocks;
        }

        void add_edge(const int start_id, const int end_id) {
            for (auto& node : nodes) {
                if (node.id == start_id)
                    node.successors.push_back(end_id);

                if (node.id == end_id)
                    node.predecessors.push_back(start_id);
            }
        }

        void add_all_edges() {
            if (nodes.size() < 2) {
                add_edge(ENTRY, EXIT);
                return;
            }

            add_edge(ENTRY, 1);
            for (const auto& node : nodes) {
                int next_id;

                if (node.id == ENTRY || node.id == EXIT)
                    continue;

                //reached the last node
                if (node.id == id_counter)
                    next_id = EXIT;
                else
                    next_id = node.id + 1;

                auto last_instruction = node.instructions.back();

                if (adapter::is_ret(last_instruction)) {
                    add_edge(node.id, EXIT);
                    continue;
                }

                if (adapter::is_unconditional_jump(last_instruction)) {
                    auto label = adapter::jump_label(last_instruction);
                    auto target_id = label_to_block_id(label);
                    add_edge(node.id, target_id);
                    continue;
                }

                if (adapter::is_conditional_jump(last_instruction)) {
                    auto label = adapter::jump_label(last_instruction);
                    auto target_id = label_to_block_id(label);
                    add_edge(node.id, target_id);
                    add_edge(node.id, next_id);
                    continue;
                }

                add_edge(node.id, next_id);
            }

        }

    };
}

