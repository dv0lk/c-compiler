#pragma once
#include <optional>
#include <unordered_set>

#include "core/basic_block.hpp"

namespace compiler {
    template <typename InstrType>
    struct Node {
        using bb_t = BasicBlock<InstrType>;

        std::optional<bb_t> block;
        std::unordered_set<size_t> successors;
        std::unordered_set<size_t> predecessors;

        Node() = default;

        explicit Node(bb_t block): block(std::move(block)) { }

        void add_successor(const size_t id) {
            successors.insert(id);
        }

        void add_predecessor(const size_t id) {
            predecessors.insert(id);
        }

        void remove_successor(const size_t id) {
            successors.erase(id);
        }

        void remove_predecessor(const size_t id) {
            predecessors.erase(id);
        }

        void clear() {
            successors.clear();
            predecessors.clear();
        }

        [[nodiscard]] bool has_block() const {
            return block.has_value();
        }

        [[nodiscard]] bool empty() const {
            if (!has_block()) {
                return true;
            }
            return block->instructions().empty();
        }
    };
} // namespace compiler
