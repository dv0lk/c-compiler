#pragma once

#include <memory>
#include <vector>
#include <ranges>
#include <unordered_set>

#include "adapter/ir_adapter.hpp"
#include "node_t.hpp"

namespace compiler::cfg {
    //TODO currently we generate IR and CFG for the whole program
    // It would be better if we generate vector of ir functions. And then generate cfg for each function
    class cfg {
    public:
        static constexpr size_t START_NODE = 0;
        static constexpr size_t EXIT_NODE = std::numeric_limits<size_t>::max() - 1;
        static constexpr size_t INVALID_NODE = std::numeric_limits<size_t>::max();

    private:
        //TODO should probably do something better
        std::unordered_map<size_t, Node> nodes_;
        std::unordered_map<std::string, size_t> label_cache_;
        size_t next_node_id_ = 0;

    public:
        cfg() = default;

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

        void build_nodes(const std::vector<ir::basic_block_t> &blocks) {
            nodes_[START_NODE] = Node{};
            nodes_[EXIT_NODE] = Node{};

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

                if (!curr_block->has_terminator()) {
                    add_edge(current_id, node_ids[i + 1]);
                    continue;
                }

                //TODO add adapter so this will work with IR instruction and Asm instructions
                const auto &last_instruction = curr_block->back();
                if (last_instruction.holds<ir::return_>()) {
                    add_edge(current_id, EXIT_NODE);
                }

                if (const auto &jmp = last_instruction.get_if<ir::jump>()) {
                    auto label = jmp->target_label.name;
                    add_edge(current_id, find_by_label(label));
                }

                if (const auto &jmp = last_instruction.get_if<ir::jump_if_zero>()) {
                    auto label = jmp->target_label.name;
                    add_edge(current_id, find_by_label(label));
                    add_edge(current_id, node_ids[i + 1]);
                }

                if (const auto &jmp = last_instruction.get_if<ir::jump_if_not_zero>()) {
                    auto label = jmp->target_label.name;
                    add_edge(current_id, find_by_label(label));
                    add_edge(current_id, node_ids[i + 1]);
                }
            }
        }

        [[nodiscard]] Node *find_node(const std::size_t id) {
            const auto it = nodes_.find(id);
            return it != nodes_.end() ? &it->second : nullptr;
        }

        [[nodiscard]] std::size_t find_by_label(const std::string &label) const {
            auto it = label_cache_.find(label);
            if (it != label_cache_.end()) {
                return it->second;
            }
            throw std::runtime_error("Encountered lable that was not in cache");
        }

        size_t add_node(const ir::basic_block_t &block) {
            const size_t id = next_node_id_++;

            label_cache_[block.name()] = id;

            nodes_[id] = Node(block);
            return id;
        }

    public:
        //TODO move this to static function
        void generate_cfg(const std::vector<ir::basic_block_t> &blocks) {
            build_nodes(blocks);
        }
    };
}
