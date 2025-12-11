#pragma once
#include <string>
#include <vector>

#include "ir_instruction.hpp"

namespace compiler::ir {
    class ir_basic_block {
    private:
        std::string name_;
        std::vector<ir_instruction> instructions_;

    public:
        explicit ir_basic_block(std::string block_name)
            : name_(std::move(block_name)) {
        }

        void add_instruction(ir_instruction instruction) {
            instructions_.push_back(std::move(instruction));
        }

        template<typename T>
        void add_instruction(T instruction) {
            instructions_.push_back(ir_instruction{std::move(instruction)});
        }

        [[nodiscard]] const std::string &name() const noexcept { return name_; }
        [[nodiscard]] const std::vector<ir_instruction> &instructions() const noexcept { return instructions_; }
        [[nodiscard]] bool is_empty() const noexcept { return instructions_.empty(); }
    };
}
