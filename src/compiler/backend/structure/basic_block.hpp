#pragma once
#include "basic_block.hpp"
#include <vector>

namespace compiler {
    template <typename InstrType>
    class BasicBlock {
    public:
        BasicBlock() = default;

        void push_back(const InstrType& instr) {
            instructions_.push_back(instr);
        }

        void push_front(const InstrType& instr) {
            instructions_.insert(instructions_.begin(), instr);
        }

        template <typename... Args>
        void emplace_back(Args&&... args) {
            instructions_.emplace_back(std::forward<Args>(args)...);
        }

        void insert(size_t position, const InstrType& instr) {
            instructions_.insert(instructions_.begin() + position, instr);
        }

        [[nodiscard]] bool empty() const {
            return instructions_.empty();
        }
        [[nodiscard]] size_t size() const {
            return instructions().size();
        };

        [[nodiscard]] std::vector<InstrType>& instructions() {
            return instructions_;
        }
        [[nodiscard]] const std::vector<InstrType>& instructions() const {
            return instructions_;
        }

        [[nodiscard]] auto front() {
            return instructions_.front();
        }
        [[nodiscard]] auto back() {
            return instructions_.back();
        }

        [[nodiscard]] auto begin() {
            return instructions_.begin();
        }
        [[nodiscard]] auto end() {
            return instructions_.end();
        }

    private:
        std::vector<InstrType> instructions_;
    };
} // namespace compiler
