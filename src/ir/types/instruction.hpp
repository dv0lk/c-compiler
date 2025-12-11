#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "lexer/token.h"
#include "ir_value.hpp"

namespace compiler::ir {
    struct return_ {
        ir_value value;
    };

    struct binary {
        token_t op;
        ir_value left;
        ir_value right;
        ir_value result;
    };

    struct unary {
        token_t op;
        ir_value value;
        ir_value result;
    };

    struct copy {
        ir_value destination;
        ir_value source;

        friend bool operator==(const copy &lhs, const copy &rhs) {
            return lhs.destination == rhs.destination
                   && lhs.source == rhs.source;
        }
    };

    struct label {
        std::string name;
    };

    struct jump {
        label target_label;
    };

    struct jump_if_zero {
        ir_value condition;
        label target_label;
    };

    struct jump_if_not_zero {
        ir_value condition;
        label target_label;
    };

    struct ir_call {
        std::string function_name;
        std::vector<ir_value> arguments;
        ir_value destination;
    };


    class instruction {
    private:
        using variant_t = std::variant<return_, binary, unary, copy, label, jump, jump_if_zero,
            jump_if_not_zero, ir_call>;

        variant_t data_;

    public:
        //TODO add good constructors
        explicit instruction(variant_t &&variant) : data_(std::move(variant)) {
        }

        template<typename T>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        template<typename T>
        [[nodiscard]] const T *get_if() const {
            return std::get_if<T>(&data_);
        }

        template<typename Visitor>
        auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };
}
