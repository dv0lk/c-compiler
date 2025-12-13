#pragma once
#include <string>
#include <vector>

#include "instruction.hpp"

namespace compiler::ir {
    class basic_block_t {
    private:
        std::string name_;
        std::vector<instruction> instructions_;

    public:
        explicit basic_block_t(std::string block_name)
            : name_(std::move(block_name)) {
        }

        void push_instr(instruction new_instruction) {
            instructions_.push_back(std::move(new_instruction));
        }

        template<typename T>
        void push_instr(T new_instruction) {
                        instructions_.push_back(instruction{std::move(new_instruction)});
        }

        [[nodiscard]] const std::string &name() const noexcept { return name_; }
        [[nodiscard]] const std::vector<instruction> &instructions() const noexcept { return instructions_; }
        [[nodiscard]] bool empty() const noexcept { return instructions_.empty(); }
    };
}
