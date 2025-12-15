#pragma once
#include <variant>

template<typename... InstrTypes>
class InstructionBase {
private:
    using variant_t = std::variant<InstrTypes...>;
    variant_t data_;

public:
    template<typename T>
    requires (std::is_same_v<std::decay_t<T>, InstrTypes> || ...)
    explicit InstructionBase(T&& data)
        : data_(std::forward<T>()(data)) {
    }

    virtual ~InstructionBase() = default;

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
