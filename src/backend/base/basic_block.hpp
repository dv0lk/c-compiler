#pragma once
#include <vector>
#include "basic_block.hpp"

namespace compiler::base {
    template<typename InstrType>
    class BasicBlock {
    private:
        std::vector<InstrType> instructions_;

    public:
        BasicBlock() = default;

        void add_instruction(const InstrType &instruction) {
            instructions_.emplace_back(instruction);
        }

        template<typename T>
        void add_instruction(T &&instruction) {
            instructions_.emplace_back(std::forward<T>(instruction));
        }

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

        [[nodiscard]] bool empty() const {
            return instructions_.empty();
        }

        [[nodiscard]] auto begin() {
            return instructions_.begin();
        }

        [[nodiscard]] auto begin() const {
            return instructions_.begin();
        }

        [[nodiscard]] auto end() {
            return instructions_.end();
        }

        [[nodiscard]] auto end() const {
            return instructions_.end();
        }
    };
}
