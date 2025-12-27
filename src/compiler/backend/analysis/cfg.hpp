#pragma once

#include <algorithm>
#include <ranges>
#include <span>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "core/basic_block.hpp"
#include "core/function.hpp"
#include "node.hpp"
#include "traits/traits.hpp"

namespace compiler {
    static constexpr size_t START_NODE = 0;
    static constexpr size_t EXIT_NODE = std::numeric_limits<size_t>::max() - 1;

    template <typename InstrType>
    class CFG {
    public:
        using traits = InstructionTrait<InstrType>;
        using bb_t = BasicBlock<InstrType>;
        using func_t = Function<InstrType>;
        using node_t = Node<InstrType>;

    private:
        std::string name_;
        std::unordered_map<size_t, node_t> nodes_;
        std::unordered_map<std::string, size_t> label_cache_;
        size_t next_node_id_ = 1;

    public:
        CFG() = default;

        [[nodiscard]] auto get_nodes() {
            return std::views::values(nodes_);
        }

        [[nodiscard]] auto get_nodes() const {
            return std::views::values(nodes_);
        }

        [[nodiscard]] auto get_block_ids() {
            return std::views::keys(nodes_);
        }

        [[nodiscard]] auto get_block_ids() const {
            return std::views::keys(nodes_);
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

        void build_nodes(std::vector<bb_t> blocks) {
            nodes_[START_NODE] = node_t{};
            nodes_[EXIT_NODE] = node_t{};

            std::vector<size_t> block_ids;

            block_ids.push_back(START_NODE);
            for (auto& block : blocks) {
                block_ids.push_back(add_node(std::move(block)));
            }
            block_ids.push_back(EXIT_NODE);

            build_edges(block_ids);
        }

        void build_edges(const std::vector<size_t>& node_ids) {
            for (size_t i = 0; i < node_ids.size(); ++i) {
                const auto current_id = node_ids[i];

                if (current_id == START_NODE) {
                    add_edge(START_NODE, node_ids[i + 1]);
                }

                if (current_id == EXIT_NODE) {
                    continue;
                }

                const auto& current_node = nodes_[current_id];

                if (!current_node.has_block()) {
                    continue;
                }

                const auto& curr_block = current_node.block;

                if (curr_block->empty()) {
                    continue;
                }

                const auto& last_instruction = curr_block->instructions().back();

                if (!traits::is_terminator(last_instruction)) {
                    add_edge(current_id, node_ids[i + 1]);
                    continue;
                }

                if (traits::is_return(last_instruction)) {
                    add_edge(current_id, EXIT_NODE);
                    continue;
                }

                if (traits::is_unconditional_jump(last_instruction)) {
                    auto label = traits::get_jump_target(last_instruction);
                    add_edge(current_id, find_by_label(label));
                    continue;
                }

                if (traits::is_conditional_jump(last_instruction)) {
                    auto label = traits::get_jump_target(last_instruction);
                    add_edge(current_id, find_by_label(label));
                    add_edge(current_id, node_ids[i + 1]);
                }
            }
        }

        [[nodiscard]] node_t* find_node(const std::size_t id) {
            const auto it = nodes_.find(id);
            return it != nodes_.end() ? &it->second : nullptr;
        }

        [[nodiscard]] const node_t* find_node(const std::size_t id) const {
            const auto it = nodes_.find(id);
            return it != nodes_.end() ? &it->second : nullptr;
        }

        [[nodiscard]] std::size_t find_by_label(const std::string& label) const {
            auto it = label_cache_.find(label);
            if (it != label_cache_.end()) {
                return it->second;
            }
            throw std::runtime_error("Encountered label that was not in cache");
        }

        size_t add_node(bb_t block) {
            const size_t id = next_node_id_++;

            // cache label, labels should always be the first instruction in bb
            if (!block.empty() && traits::is_label(block.instructions().front())) {
                const auto& label = traits::get_label_name(block.instructions().front());
                label_cache_[label] = id;
            }

            nodes_[id] = Node(std::move(block));
            return id;
        }

        static CFG from_bbs(std::vector<bb_t> blocks) {
            CFG cfg;
            cfg.build_nodes(std::move(blocks));
            return cfg;
        }

        static std::vector<bb_t> compute_basic_blocks(std::span<const InstrType> instructions) {
            if (instructions.empty())
                return {};

            std::unordered_set<size_t> leaders;
            leaders.insert(0);

            for (size_t i = 0; i < instructions.size(); ++i) {
                const auto& instr = instructions[i];

                if (traits::is_terminator(instr) && i + 1 < instructions.size()) {
                    leaders.insert(i + 1);
                }

                if (traits::is_label(instr)) {
                    leaders.insert(i);
                }
            }

            std::vector<bb_t> blocks;
            std::vector<size_t> sorted_leaders(leaders.begin(), leaders.end());
            std::sort(sorted_leaders.begin(), sorted_leaders.end());

            for (size_t i = 0; i < sorted_leaders.size(); ++i) {
                size_t start = sorted_leaders[i];
                size_t end = (i + 1 < sorted_leaders.size()) ? sorted_leaders[i + 1] : instructions.size();

                bb_t block;
                for (size_t j = start; j < end; ++j) {
                    block.push_back(instructions[j]);
                }
                blocks.push_back(std::move(block));
            }

            return blocks;
        }

        static CFG from_instructions(std::span<const InstrType> instructions) {
            CFG cfg;
            auto blocks = compute_basic_blocks(instructions);
            cfg.build_nodes(std::move(blocks));
            return cfg;
        }

        static CFG from_function(const func_t& function) {
            return from_instructions(function.instructions());
        }
    };
} // namespace compiler
