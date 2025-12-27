#pragma once
#include <utility>
#include <variant>
#include "operations.hpp"

namespace compiler::ir {
    class Instruction {
    private:
        using variant_t = std::variant<Return, Binary, Unary, Copy, Label, Jump, JumpIfZero,
            JumpIfNotZero, FunctionCall>;

        variant_t data_;

    public:
        explicit Instruction(variant_t &&variant) : data_(std::move(variant)) {
        }

        Instruction(const Instruction &other) = default;

        Instruction(Instruction &&other) noexcept = default;

        Instruction &operator=(const Instruction &other) = default;

        Instruction &operator=(Instruction &&other) noexcept = default;

        bool operator==(const Instruction &) const = default;

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
