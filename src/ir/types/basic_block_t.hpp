#pragma once
#include <span>
#include <string>
#include <vector>

#include "instruction.hpp"

namespace compiler::ir {
    class basic_block_t {
    private:
        std::vector<instruction> instructions_;

    public:
        basic_block_t() = default;

        // [[nodiscard]] const std::string &name() const {
        //     return name_;
        // }

        [[nodiscard]] bool empty() const {
            return instructions_.empty();
        }

        [[nodiscard]] std::span<instruction> instructions() {
            return instructions_;
        }

        [[nodiscard]] std::span<const instruction> instructions() const {
            return instructions_;
        }

        [[nodiscard]] const instruction &front() const {
            if (empty()) {
                throw std::runtime_error("Cannot call front() on an empty basic block");
            }
            return instructions_.front();
        }

        [[nodiscard]] const instruction &back() const {
            if (empty()) {
                throw std::runtime_error("Cannot call back() on an empty basic block");
            }
            return instructions_.back();
        }

        void append(instruction new_instruction) {
            instructions_.push_back(std::move(new_instruction));
        }

        template<typename T>
        void append(T &&new_instruction) {
            instructions_.push_back(instruction{std::forward<T>(new_instruction)});
        }

        [[nodiscard]] std::optional<std::string> block_label() const {
            if (!empty()) {
                if (const auto &label = front().get_if<ir::label>()) {
                    return label->name;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] bool has_terminator() const {
            return !empty() && back().is_terminator();
        }

        [[nodiscard]] std::string get_terminator_label() const {
            const auto last_instruction = back();


            if (const auto &jmp = last_instruction.get_if<ir::jump>()) {
                return jmp->target_label.name;
            }

            if (const auto &jmp = last_instruction.get_if<ir::jump_if_zero>()) {
                return jmp->target_label.name;
            }

            if (const auto &jmp = last_instruction.get_if<ir::jump_if_not_zero>()) {
                return jmp->target_label.name;
            }

            throw std::runtime_error("Unexpected instruction");
        }

        [[nodiscard]] auto begin() const {
            return instructions_.begin();
        }

        [[nodiscard]] auto end() const {
            return instructions_.end();
        }

        [[nodiscard]] auto begin() {
            return instructions_.begin();
        }

        [[nodiscard]] auto end() {
            return instructions_.end();
        }
    };
}
