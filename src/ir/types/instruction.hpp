#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "lexer/token.h"
#include "value_t.hpp"

namespace compiler::ir {
    struct return_ {
        value_t value;

        bool operator==(const return_ &) const = default;
    };

    struct binary {
        token_t op;
        value_t left;
        value_t right;
        value_t result;

        bool operator==(const binary &) const = default;
    };

    struct unary {
        token_t op;
        value_t value;
        value_t result;

        bool operator==(const unary &) const = default;
    };

    struct copy {
        value_t destination;
        value_t source;

        bool operator==(const copy &) const = default;
    };

    struct label {
        std::string name;

        bool operator==(const label &) const = default;
    };

    struct jump {
        label target_label;

        bool operator==(const jump &) const = default;
    };

    struct jump_if_zero {
        value_t condition;
        label target_label;

        bool operator==(const jump_if_zero &) const = default;
    };

    struct jump_if_not_zero {
        value_t condition;
        label target_label;

        bool operator==(const jump_if_not_zero &) const = default;
    };

    struct func_call {
        std::string function_name;
        std::vector<value_t> arguments;
        value_t destination;

        bool operator==(const func_call &) const = default;
    };


    class instruction {
    private:
        using variant_t = std::variant<return_, binary, unary, copy, label, jump, jump_if_zero,
            jump_if_not_zero, func_call>;

        variant_t data_;

    public:
        //TODO add good constructors
        explicit instruction(variant_t &&variant) : data_(std::move(variant)) {
        }

        bool operator==(const instruction &) const = default;

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

        [[nodiscard]] bool is_terminator() const {
            return holds<return_>() || holds<jump>() || holds<jump_if_zero>() || holds<jump_if_not_zero>();
        }
    };
}
