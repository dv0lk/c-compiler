#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "lexer/token.hpp"

namespace compiler::ast::expr {
    class expr;
    using expr_ptr = std::shared_ptr<expr>;

    template<typename T, typename... Args>
    [[nodiscard]] expr_ptr make_expr(Args &&... args) {
        return std::make_shared<expr>(T{std::forward<Args>(args)...});
    }

    struct Literal {
        int value;
    };

    struct Binary {
        expr_ptr left;
        TokenType op;
        expr_ptr right;
    };

    struct Unary {
        TokenType op;
        expr_ptr value;
    };

    struct Logical {
        expr_ptr left;
        TokenType op;
        expr_ptr right;
    };

    struct Grouping {
        expr_ptr expr;
    };

    struct Assignment {
        std::string name;
        expr_ptr value;
    };

    struct Variable {
        std::string name;
    };

    struct Call {
        std::string identifier;
        std::vector<expr_ptr> arguments;
    };


    class expr {
    private:
        using variant_t = std::variant<Literal, Binary, Grouping, Unary, Logical, Variable
            , Assignment, Call>;
        variant_t data_;

    public:
        template<typename T>
        explicit expr(T &&value) : data_(std::forward<T>(value)) {
        }

        template<typename T>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        template<typename T>
        [[nodiscard]] const T *get_if() const {
            return std::get_if<T>(&data_);
        }

        template<typename Visitor>
        auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };
}
