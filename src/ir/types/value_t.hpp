#pragma once
#include <string>
#include <variant>


namespace compiler::ir {
    class value_t {
    private:
        std::variant<std::string, int> value_;

    public:
        value_t() = default;

        explicit value_t(int i)
            : value_(i) {}

        explicit value_t(std::string str)
            : value_(std::move(str)) {}

        template <typename T>
        [[nodiscard]] const T& get() const {
            return std::get<T>(value_);
        }

        [[nodiscard]] bool is_constant() const {
            return std::holds_alternative<int>(value_);
        }

        [[nodiscard]] std::string value_to_string() const {
            if (is_constant()) {
                return std::to_string(get<int>());
            }
            return get<std::string>();
        }

        bool operator==(const value_t& source) const = default;
    };
}