#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "expr.hpp"
#include "lexer/token.h"

namespace compiler::ast::stmt {
    class stmt;
    using stmt_ptr = std::shared_ptr<stmt>;

    template<typename T, typename... Args>
    [[nodiscard]] stmt_ptr make_stmt(Args &&... args) {
        return std::make_shared<stmt>(T{std::forward<Args>(args)...});
    }

    struct return_ {
        expr::expr_ptr value;
    };

    struct expression {
        expr::expr_ptr expr;
    };

    struct if_ {
        expr::expr_ptr condition;
        stmt_ptr then_branch;
        std::optional<stmt_ptr> else_branch;
    };

    struct while_ {
        expr::expr_ptr condition;
        stmt_ptr body;
    };

    struct function_param {
        std::string name;
        token_t type;
    };

    struct function_decl {
        token_t return_type;
        std::string function_name;
        std::vector<function_param> params;
        stmt_ptr body;
    };

    struct block {
        std::vector<stmt_ptr> statements;
    };

    struct variable {
        std::string name;
        std::optional<expr::expr_ptr> initializer;
    };

    struct for_loop {
        variable variable;
        expr::expr_ptr condition;
        stmt_ptr body;
    };

    class stmt {
    private:
        using variant_t = std::variant<return_, expression, if_, while_, function_param,
            function_decl, block, variable>;
        variant_t data_;

    public:
        template<typename T>
        explicit stmt(T &&value) : data_(std::forward<T>(value)) {
        }

        template<typename T>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        template<typename T>
        [[nodiscard]] const T &get_if() const {
            return std::get_if<T>(data_);
        }

        template<typename Visitor>
        constexpr auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };
}
