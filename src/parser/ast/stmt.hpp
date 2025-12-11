#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "expr.hpp"
#include "lexer/token.h"

namespace compiler::ast {
    class stmt;
    using stmt_ptr = std::shared_ptr<stmt>;

    template<typename T, typename... Args>
    [[nodiscard]] stmt_ptr make_stmt(Args &&... args) {
        return std::make_shared<stmt>(T{std::forward<Args>(args)...});
    }

    struct return_stmt {
        expr_ptr value;
    };

    struct expression_stmt {
        expr_ptr expr;
    };

    struct if_stmt {
        expr_ptr condition;
        stmt_ptr then_branch;
        std::optional<stmt_ptr> else_branch;
    };

    struct while_stmt {
        expr_ptr condition;
        stmt_ptr body;
    };

    struct function_param_stmt {
        std::string name;
        token_t type;
    };

    struct function_decl_stmt {
        token_t return_type;
        std::string function_name;
        std::vector<function_param_stmt> params;
        stmt_ptr body;
    };

    struct block_stmt {
        std::vector<stmt_ptr> statements;
    };

    struct variable_stmt {
        std::string name;
        std::optional<expr_ptr> initializer;
    };

    struct for_loop_stmt {
        variable_stmt variable;
        expr_ptr condition;
        stmt_ptr body;
    };

    class stmt {
    private:
        using variant_t = std::variant<return_stmt, expression_stmt, if_stmt, while_stmt, function_param_stmt,
            function_decl_stmt, block_stmt, variable_stmt>;
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
