#pragma once
#include <string>
#include <variant>

namespace compiler::ir {
    class Operand {
    private:
        using constant_t = int;
        using variable_t = std::string;

        std::variant<constant_t, variable_t> value_;

    public:
        Operand() = default;

        Operand(constant_t constant): value_(constant) { }

        Operand(variable_t variable): value_(std::move(variable)) { }

        [[nodiscard]] constexpr bool is_constant() const {
            return std::holds_alternative<constant_t>(value_);
        }

        [[nodiscard]] constexpr bool is_variable() const {
            return std::holds_alternative<variable_t>(value_);
        }

        [[nodiscard]] constant_t get_constant() const {
            return std::get<constant_t>(value_);
        }

        [[nodiscard]] const variable_t& get_variable() const {
            return std::get<variable_t>(value_);
        }

        [[nodiscard]] std::string to_string() const {
            if (is_constant()) {
                return std::to_string(get_constant());
            }
            return get_variable();
        }

        bool operator==(const Operand& source) const = default;
    };
} // namespace compiler::ir
