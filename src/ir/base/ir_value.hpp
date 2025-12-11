#pragma once
#include <string>
#include <variant>

class ir_value {
private:
    std::variant<std::string, int> value_;

public:
    ir_value() = default;

    explicit ir_value(int i)
        : value_(i) {}

    explicit ir_value(std::string str)
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

    bool operator==(const ir_value& source) const = default;
};
