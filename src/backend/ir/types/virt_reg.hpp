#pragma once
#include <string>
#include <variant>


namespace compiler::ir {
    //TODO make actual virtual register or something, this kinda sucks rn
    class VirtualReg {
    private:
        using constant_t = int;
        using variable_t = std::string;

        std::variant<constant_t, variable_t> value_;

    public:
        VirtualReg() = default;

        //TODO figure this shit out, casuses compiler errors
        // value_t & operator=(const value_t &other) = default;
        // value_t & operator=(value_t &&other) = default;

        explicit VirtualReg(constant_t constant)
            : value_(constant) {}

        explicit VirtualReg(variable_t variable)
            : value_(std::move(variable)) {}

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

        bool operator==(const VirtualReg& source) const = default;
    };
}