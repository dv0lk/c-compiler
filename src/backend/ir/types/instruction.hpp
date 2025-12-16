#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "lexer/token.h"
#include "virt_reg.hpp"

namespace compiler::ir {
    struct return_ {
        VirtualReg value;

        bool operator==(const return_ &) const = default;
    };

    struct binary {
        token_t op;
        VirtualReg left;
        VirtualReg right;
        VirtualReg result;

        bool operator==(const binary &) const = default;
    };

    struct unary {
        token_t op;
        VirtualReg value;
        VirtualReg result;

        bool operator==(const unary &) const = default;
    };

    struct copy {
        VirtualReg destination;
        VirtualReg source;

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
        VirtualReg condition;
        label target_label;

        bool operator==(const jump_if_zero &) const = default;
    };

    struct jump_if_not_zero {
        VirtualReg condition;
        label target_label;

        bool operator==(const jump_if_not_zero &) const = default;
    };

    struct func_call {
        std::string function_name;
        std::vector<VirtualReg> arguments;
        VirtualReg destination;

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

        instruction(const instruction &other) = default;

        instruction(instruction &&other) noexcept = default;

        instruction& operator=(const instruction& other) = default;
        instruction& operator=(instruction&& other) noexcept = default;

        bool operator==(const instruction &) const = default;

        template<typename... Ts>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return (std::holds_alternative<Ts>(data_) || ...);
        }

        template<typename T>
        [[nodiscard]] const T *get_if() const {
            return std::get_if<T>(&data_);
        }

        template<typename T>
        [[nodiscard]] T *get_if() {
            return std::get_if<T>(&data_);
        }

        template<typename Visitor>
        auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }

        template<typename Visitor>
        auto visit(Visitor &&visitor) {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };

}
