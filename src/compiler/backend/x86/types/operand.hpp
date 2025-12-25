#pragma once
#include <variant>
#include <type_traits>

#include "regs.hpp"
#include "operand.hpp"

namespace compiler::x86 {
    struct Imm {
        int value;

        explicit Imm(const int value)
            : value(value) {
        }

        friend bool operator==(const Imm &lhs, const Imm &rhs) {
            return lhs.value == rhs.value;
        }
    };

    struct LabelOp {
        std::string name;

        LabelOp() = default;

        LabelOp(std::string name)
            : name(std::move(name)) {
        }

        friend bool operator==(const LabelOp &lhs, const LabelOp &rhs) {
            return lhs.name == rhs.name;
        }
    };

    struct Mem {
        int offset;

        Mem() = default;

        explicit Mem(const int offset) : offset(offset) {
        }

        friend bool operator==(const Mem &lhs, const Mem &rhs) {
            return lhs.offset == rhs.offset;
        }
    };

    struct Register {
        RegType type_;

        explicit Register(const RegType type) : type_(type) {
        }

        friend bool operator==(const Register &lhs, const Register &rhs) {
            return lhs.type_ == rhs.type_;
        }
    };

    struct PseudoRegister {
        std::string name;

        PseudoRegister() = default;

        explicit PseudoRegister(const std::string &name)
            : name(name) {
        }

        friend bool operator==(const PseudoRegister &lhs, const PseudoRegister &rhs) = default;
    };

    class Operand {
    public:
        constexpr Operand() = default;

        template<typename T>
        requires (!std::same_as<std::decay_t<T>, Operand>)
        constexpr Operand(T &&operand) : data_(std::forward<T>(operand)) {
        }

        template<typename T>
        requires (!std::same_as<std::decay_t<T>, Operand>)
        Operand &operator=(T &&operand) {
            data_ = std::forward<T>(operand);
            return *this;
        }

        friend bool operator==(const Operand &, const Operand &) = default;

        [[nodiscard]] bool empty() const noexcept {
            return std::holds_alternative<std::monostate>(data_);
        }

        template<typename T>
        [[nodiscard]] T *get_if() noexcept {
            return std::get_if<T>(&data_);
        }

        template<typename T>
        [[nodiscard]] const T *get_if() const noexcept {
            return std::get_if<T>(&data_);
        }


        template<typename... Ts>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return (std::holds_alternative<Ts>(data_) || ...);
        }

        template<typename Visitor>
        decltype(auto) visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }

        template<typename Visitor>
        decltype(auto) visit(Visitor &&visitor) {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }

    private:
        using variant_t = std::variant<std::monostate, PseudoRegister, Register, Imm, LabelOp, Mem>;
        variant_t data_;
    };
}
