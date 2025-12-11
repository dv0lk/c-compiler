#pragma once
#include <string>
#include <vector>

#include "instruction.hpp"

namespace compiler::ir {
    class basic_block {
    private:
        std::string name_;
        std::vector<instruction> instructions_;

    public:
        explicit basic_block(std::string block_name)
            : name_(std::move(block_name)) {
        }

        void add_instruction(instruction new_instruction) {
            instructions_.push_back(std::move(new_instruction));
        }

        template<typename T>
        void add_instruction(T new_instruction) {
                        instructions_.push_back(instruction{std::move(new_instruction)});
        }

        [[nodiscard]] const std::string &name() const noexcept { return name_; }
        [[nodiscard]] const std::vector<instruction> &instructions() const noexcept { return instructions_; }
        [[nodiscard]] bool is_empty() const noexcept { return instructions_.empty(); }
    };
}
