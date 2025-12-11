#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "lexer/token.h"

namespace compiler::ast {
    class expr;
    using expr_ptr = std::shared_ptr<expr>;

    template<typename T, typename... Args>
    [[nodiscard]] expr_ptr make_expr(Args &&... args) {
        return std::make_shared<expr>(T{std::forward<Args>(args)...});
    }

    struct literal_expr {
        int value;
    };

    struct binary_expr {
        expr_ptr left;
        token_t op;
        expr_ptr right;
    };

    struct unary_expr {
        token_t op;
        expr_ptr value;
    };

    struct logical_expr {
        expr_ptr left;
        token_t op;
        expr_ptr right;
    };

    struct grouping_expr {
        expr_ptr expr;
    };

    struct assignment_expr {
        std::string name;
        expr_ptr value;
    };

    struct variable_expr {
        std::string name;
    };

    struct call_expr {
        std::string identifier;
        std::vector<expr_ptr> arguments;
    };


    class expr {
    private:
        using variant_t = std::variant<literal_expr, binary_expr, grouping_expr, unary_expr, logical_expr, variable_expr
            , assignment_expr, call_expr>;
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
