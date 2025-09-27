#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "lexer/token.h"

namespace compiler::ir {
    class ir_value {
    private:
        std::variant<std::string, int> value;

    public:
        ir_value() = default;

        explicit ir_value(int i)
            : value(i) {}

        explicit ir_value(const std::string& str)
            : value(str) {}

        template <typename T>
        [[nodiscard]] const T& get() const {
            return std::get<T>(value);
        }

        [[nodiscard]] bool is_constant() const {
            return std::holds_alternative<int>(value);
        }

        [[nodiscard]] std::string value_to_string() const {
            if (is_constant()) {
                return std::to_string(get<int>());
            }
            return get<std::string>();
        }

        bool operator==(const ir_value& source) const = default;
    };

    struct ir_return;
    struct ir_binary;
    struct ir_unary;
    struct ir_copy;
    struct ir_label;
    struct ir_jump;
    struct ir_jump_if_zero;
    struct ir_jump_if_not_zero;
    struct ir_call;

    using ir_instruction = std::variant<ir_return, ir_binary, ir_unary, ir_copy, ir_label, ir_jump, ir_jump_if_zero, ir_jump_if_not_zero, ir_call>;

    struct ir_return {
        ir_value value;
    };

    struct ir_binary {
        token_type op;
        ir_value left;
        ir_value right;
        ir_value result;
    };

    struct ir_unary {
        token_type op;
        ir_value value;
        ir_value result;
    };

    struct ir_copy {
        ir_value destination;
        ir_value source;

        friend bool operator==(const ir_copy& lhs, const ir_copy& rhs) {
            return lhs.destination == rhs.destination
                   && lhs.source == rhs.source;
        }

        friend bool operator!=(const ir_copy& lhs, const ir_copy& rhs) {
            return !(lhs == rhs);
        }
    };

    struct ir_label {
        std::string name;
    };

    struct ir_jump {
        ir_label label;
    };

    struct ir_jump_if_zero {
        ir_value condition;
        ir_label label;
    };

    struct ir_jump_if_not_zero {
        ir_value condition;
        ir_label label;
    };

    struct ir_call {
        std::string function_name;
        std::vector<ir_value> arguments;
        ir_value destination;
    };


    class ir_basic_block {
    public:
        std::string name;
        std::vector<ir_instruction> instructions;

        explicit ir_basic_block(std::string block_name)
            : name(std::move(block_name)) {}

        void add_instruction(ir_instruction inst) {
            instructions.push_back(std::move(inst));
        }

        [[nodiscard]] const std::string& get_name() const {
            return name;
        }

        [[nodiscard]] std::vector<ir_instruction> get_instructions() const {
            return instructions;
        }

        [[nodiscard]] bool is_empty() const {
            return instructions.empty();
        }

    };
}
