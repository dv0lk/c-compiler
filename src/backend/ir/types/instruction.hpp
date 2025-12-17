#pragma once
#include <utility>
#include <variant>
#include "types.hpp"

namespace compiler::ir {
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

        instruction &operator=(const instruction &other) = default;

        instruction &operator=(instruction &&other) noexcept = default;

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
