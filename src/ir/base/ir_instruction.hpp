#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "lexer/token.h"
#include "ir_types.hpp"

namespace compiler::ir {
    class ir_instruction {
    private:
        using variant_t = std::variant<ir_return, ir_binary, ir_unary, ir_copy, ir_label, ir_jump, ir_jump_if_zero,
            ir_jump_if_not_zero, ir_call>;

        variant_t data_;

    public:
        //TODO add good constructors
        explicit ir_instruction(variant_t&& variant) : data_(std::move(variant)) {
        }

        template<typename T>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        template<typename T>
        [[nodiscard]] const T &get_if() const {
            return std::get_if<T>(data_);
        }

        template<typename Visitor>
        auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };
}
