#pragma once
#include <memory>
#include <unordered_set>

#include "ir/types/basic_block_t.hpp"

namespace compiler::cfg {
    // template<typename BlockType>
    struct Node {
        std::shared_ptr<ir::basic_block_t> block;
        std::unordered_set<size_t> successors;
        std::unordered_set<size_t> predecessors;

        Node() = default;

        explicit Node(const ir::basic_block_t &block) : block(std::make_shared<ir::basic_block_t>(block)) {
        }

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
            return block != nullptr;
        }

        [[nodiscard]] bool empty() const {
            if (!has_block()) {
                return true;
            }
            return block->instructions().empty();
        }
    };
}
