#pragma once
#include <type_traits>
#include "operations.hpp"

namespace compiler::x86 {
    class Instruction {
    private:
        using variant_t = std::variant<std::monostate, Mov, Add, Sub, Imul, Idiv, Push, Pop, Ret, Label, Jmp, JmpCC, Cmp, Neg, Not, cdq, SetCC, Call>;
        variant_t data_;

    public:
        Instruction() : data_(std::monostate{}) {}

        template<typename T>
        //TODO check why do we need this
        requires (!std::is_same_v<std::decay_t<T>, Instruction> && std::is_constructible_v<variant_t, T&&>)
        explicit constexpr Instruction(T &&instruction) : data_(std::forward<T>(instruction)) {
        }

        constexpr Instruction(const Instruction &other) = default;

        constexpr Instruction(Instruction &&other) noexcept : data_(std::move(other.data_)) {
        }

        Instruction &operator=(const Instruction &other) {
            if (this == &other)
                return *this;
            data_ = other.data_;
            return *this;
        }

        Instruction &operator=(Instruction &&other) noexcept {
            if (this == &other)
                return *this;
            data_ = std::move(other.data_);
            return *this;
        }

        template<typename T>
        Instruction &operator=(const T &instruction) {
            data_ = instruction;
            return *this;
        }

        template<typename T>
        Instruction &operator=(T &&instruction) {
            data_ = std::forward<T>(instruction);
            return *this;
        }

        friend bool operator==(const Instruction &lhs, const Instruction &rhs) = default;

        template<typename T>
        [[nodiscard]] T *get_if() {
            return std::get_if<T>(&data_);
        }

        template<typename T>
        [[nodiscard]] const T *get_if() const {
            return std::get_if<T>(&data_);
        }

        template<typename... Ts>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return (std::holds_alternative<Ts>(data_) || ...);
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
