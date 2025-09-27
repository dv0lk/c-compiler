#include "constant_folding.hpp"

namespace compiler {
    //TODO this might need to be node not blocks, check later
    bool ConstantFolding::apply(std::vector<ir::ir_instruction> &instructions) {
        bool changed = false;

        for (const auto& instruction : instructions) {
            changed |= process_instruction(instruction);
        }

        if (changed)
            instructions = std::move(new_instructions);

        return changed;
    }

    std::optional<int> ConstantFolding::get_constant_value(const ir::ir_value& val) {
        if (val.is_constant())
            return val.get<int>();

        return std::nullopt;
    }

    bool ConstantFolding::process_instruction(const ir::ir_instruction& inst) {
        const auto new_inst = std::visit([](auto& i) {
            return fold_instruction(i);
        }, inst);
        if (new_inst.has_value()) {
            new_instructions.emplace_back(new_inst.value());
            return true;
        }
        return false;
    }

    std::optional<ir::ir_instruction> ConstantFolding::fold_instruction(const ir::ir_binary& inst) {
        const auto left = get_constant_value(inst.left);
        const auto right = get_constant_value(inst.right);

        if (left && right) {
            const auto result = evaluate_binary(inst.op, left.value(), right.value());

            if (result.has_value()) {
                return ir::ir_copy{inst.result, ir::ir_value{result.value()}};
            }
        }
        return std::nullopt;
    }

    std::optional<ir::ir_instruction> ConstantFolding::fold_instruction(const ir::ir_unary& inst) {
        const auto constant = get_constant_value(inst.value);
        if (constant) {
            auto result = evaluate_unary(inst.op, constant.value());

            if (result.has_value()) {
                return ir::ir_copy{inst.result, ir::ir_value{result.value()}};
            }
        }
        return std::nullopt;
    }

    std::optional<int> ConstantFolding::evaluate_unary(const token_type op, const int value) {
        switch (op) {
        case token_type::Minus:
            return -value;
        case token_type::Tilde:
            return ~value;
        case token_type::Not:
            return !value ? 1 : 0;
        default:
            return std::nullopt;
        }
    }

    std::optional<int>
    ConstantFolding::evaluate_binary(const token_type op, const int left, const int right) {
        switch (op) {
        case token_type::Plus:
            return left + right;
        case token_type::Minus:
            return left - right;
        case token_type::Star:
            return left * right;
        case token_type::Slash:
            return left / right;
        case token_type::NotEqual:
            return left != right ? 1 : 0;
        case token_type::EqualEqual:
            return left == right ? 1 : 0;
        case token_type::Less:
            return left < right ? 1 : 0;
        case token_type::LessEqual:
            return left <= right ? 1 : 0;
        case token_type::Greater:
            return left > right ? 1 : 0;
        case token_type::GreaterEqual:
            return left >= right ? 1 : 0;
        default:
            return std::nullopt;
        }
    }
}

