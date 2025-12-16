#pragma once

#include <vector>
#include <ranges>
#include <unordered_set>

#include "Node.hpp"
#include "traits/traits.hpp"

namespace compiler {
    template<typename InstrType>
    class CFG {
    public:
        using Traits = base::InstructionTrait<InstrType>;
        using BlockType = base::BasicBlock<InstrType>;
        using FunctionType = base::Function<InstrType>;
        using NodeType = Node<InstrType>;

        static constexpr size_t START_NODE = 0;
        static constexpr size_t EXIT_NODE = std::numeric_limits<size_t>::max() - 1;
        static constexpr size_t INVALID_NODE = std::numeric_limits<size_t>::max();

    private:
        //TODO should probably do something better
        std::string name_;
        std::unordered_map<size_t, NodeType> nodes_;
        std::unordered_map<std::string, size_t> label_cache_;
        size_t next_node_id_ = 0;

    public:
        CFG() = default;

        auto get_nodes() {
            return std::views::values(nodes_);
        }

        void add_edge(size_t from, size_t to) {
            auto node_from = find_node(from);
            auto node_to = find_node(to);

            if (!node_from || !node_to) {
                throw std::runtime_error("Error adding an edge, one of the nodes doesn't exist");
            }

            node_from->add_successor(to);
            node_to->add_predecessor(from);
        }

        void remove_edge(const size_t from, const size_t to) {
            const auto node_from = find_node(from);
            const auto node_to = find_node(to);

            if (!node_from || !node_to) {
                throw std::runtime_error("Error deleting an edge, one of the nodes doesn't exist");
            }

            node_from->remove_successor(to);
            node_to->remove_predecessor(from);
        }

        void build_nodes(const std::span<const BlockType> &blocks) {
            nodes_[START_NODE] = NodeType{};
            nodes_[EXIT_NODE] = NodeType{};

            std::vector<size_t> block_ids;

            block_ids.push_back(START_NODE);
            for (const auto &block: blocks) {
                block_ids.push_back(add_node(block));
            }
            block_ids.push_back(EXIT_NODE);

            build_edges(block_ids);
        }

        void build_edges(const std::vector<size_t> &node_ids) {
            for (size_t i = 0; i < node_ids.size(); ++i) {
                const auto current_id = node_ids[i];

                if (current_id == START_NODE) {
                    add_edge(START_NODE, node_ids[i + 1]);
                }

                if (current_id == EXIT_NODE) {
                    continue;
                }

                const auto &current_node = nodes_[current_id];

                if (!current_node.has_block()) {
                    continue;
                }

                const auto &curr_block = current_node.block;

                if (curr_block->empty()) {
                    continue;
                }

                const auto& last_instruction = curr_block->instructions().back();

                if (!Traits::is_terminator(last_instruction)) {
                    add_edge(current_id, node_ids[i + 1]);
                    continue;
                }

                if (Traits::is_return(last_instruction)) {
                    add_edge(current_id, EXIT_NODE);
                    continue;
                }

                if (Traits::is_unconditional_jump(last_instruction)) {
                    auto label = Traits::get_jump_target(last_instruction);
                    add_edge(current_id, find_by_label(label));
                    continue;
                }

                if (Traits::is_conditional_jump(last_instruction)) {
                    auto label = Traits::get_jump_target(last_instruction);
                    add_edge(current_id, find_by_label(label));
                    add_edge(current_id, node_ids[i + 1]);
                }
            }
        }

        [[nodiscard]] NodeType *find_node(const std::size_t id) {
            const auto it = nodes_.find(id);
            return it != nodes_.end() ? &it->second : nullptr;
        }

        [[nodiscard]] std::size_t find_by_label(const std::string &label) const {
            auto it = label_cache_.find(label);
            if (it != label_cache_.end()) {
                return it->second;
            }
            throw std::runtime_error("Encountered label that was not in cache");
        }

        size_t add_node(const BlockType &block) {
            const size_t id = next_node_id_++;

            // label_cache_[block.name()] = id;
            //cache label, labels should always be the first instruction in bb
            if (!block.empty() && Traits::is_label(block.instructions().front())) {
                const auto &label = Traits::get_label_name(block.instructions().front());
                label_cache_[label] = id;
            }

            nodes_[id] = Node(block);
            return id;
        }

        static CFG from_bbs(const std::span<const BlockType>& blocks) {
            CFG cfg;
            cfg.build_nodes(blocks);
            return cfg;
        }

        // template<typename InstrType>
        static CFG from_function(const FunctionType& function) {
            return from_bbs(function.basic_blocks());
        }
    };
}
