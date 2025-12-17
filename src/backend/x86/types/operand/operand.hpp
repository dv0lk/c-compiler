#pragma once
#include <variant>
#include <type_traits>
#include "register.hpp"
#include "imm.hpp"
#include "label.hpp"
#include "operand/mem.hpp"
namespace compiler::x86 {


    class Operand {
    private:
        using variant_t = std::variant<std::monostate, PseudoRegister, Register, Imm, LabelOperand, Mem>;
        variant_t data_;

    public:
        constexpr Operand() : data_(std::monostate{}) {
        }

        template<typename T>
        constexpr Operand(T &&operand) : data_(std::forward<T>(operand)) {
        }

        constexpr Operand(const Operand &other) : data_(other.data_) {
        }

        constexpr Operand(Operand &&other) noexcept : data_(std::move(other.data_)) {
        }

        Operand &operator=(const Operand &other) {
            if (this == &other)
                return *this;
            data_ = other.data_;
            return *this;
        }

        Operand &operator=(Operand &&other) noexcept {
            if (this == &other)
                return *this;
            data_ = std::move(other.data_);
            return *this;
        }

        template<typename T>
        Operand &operator=(const T &operand) {
            data_ = operand;
            return *this;
        }

        template<typename T>
        Operand &operator=(const T &&operand) {
            data_ = std::move(operand);
            return *this;
        }

        friend bool operator==(const Operand &lhs, const Operand &rhs) = default;

        bool empty() const {
            return std::holds_alternative<std::monostate>(data_);
        }

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
