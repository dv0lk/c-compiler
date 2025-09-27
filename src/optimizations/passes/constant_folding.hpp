#pragma once
#include "ir/ir.h"
#include "lexer/token.h"


namespace compiler {
    class ConstantFolding {
    public:
        bool apply(std::vector<ir::ir_instruction> &instructions);

    private:
        std::vector<ir::ir_instruction> new_instructions;

        static std::optional<int> get_constant_value(const ir::ir_value& val);

        //default case for every instruction we don't handle
        template <typename T>
        static std::optional<ir::ir_instruction> fold_instruction(T&) {
            return std::nullopt;
        }

        bool process_instruction(const ir::ir_instruction& inst);

        //TODO add support for jump_if_zero jump_if_not_zero
        static std::optional<int> evaluate_unary(token_type op, int value);

        static std::optional<int> evaluate_binary(token_type op, int left, int right);

        static std::optional<ir::ir_instruction> fold_instruction(const ir::ir_binary& inst);

        static std::optional<ir::ir_instruction> fold_instruction(const ir::ir_unary& inst);
    };
}

