#pragma once
#include <memory>
#include <unordered_set>

#include "base/program.hpp"

namespace compiler {
    template<typename InstrType>
    struct Node {
        using BBType = base::BasicBlock<InstrType>;

        std::shared_ptr<BBType> block;
        std::unordered_set<size_t> successors;
        std::unordered_set<size_t> predecessors;

        Node() = default;

        explicit Node(const BBType &block) : block(std::make_shared<BBType>(block)) {
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
