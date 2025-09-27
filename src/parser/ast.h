#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "lexer/token.h"

namespace compiler::ast {
    struct literal_expr;
    struct binary_expr;
    struct unary_expr;
    struct grouping_expr;
    struct logical_expr;
    struct assignment_expr;
    struct variable_expr;
    struct call_expr;
    using expr = std::variant<literal_expr, binary_expr, grouping_expr, unary_expr, logical_expr, variable_expr, assignment_expr, call_expr>;
    using expr_ptr = std::shared_ptr<expr>;

    template <typename T, typename... Args>
    [[nodiscard]] expr_ptr make_expr(Args&&... args) {
        return std::make_shared<expr>(T{std::forward<Args>(args)...});
    }

    struct literal_expr {
        int value;
    };

    struct binary_expr {
        expr_ptr left;
        token_type op;
        expr_ptr right;
    };

    struct unary_expr {
        token_type op;
        expr_ptr value;
    };

    struct logical_expr {
        expr_ptr left;
        token_type op;
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

    struct return_stmt;
    struct expression_stmt;
    struct var_declaration_stmt;
    struct if_stmt;
    struct while_stmt;
    struct function_param_stmt;
    struct function_decl_stmt;
    struct block_stmt;
    struct variable_stmt;

    using stmt = std::variant<return_stmt, expression_stmt, if_stmt, while_stmt, function_param_stmt, function_decl_stmt, block_stmt, variable_stmt>;
    using stmt_ptr = std::shared_ptr<stmt>;

    template <typename T, typename... Args>
    [[nodiscard]] stmt_ptr make_stmt(Args&&... args) {
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
        token_type type;
    };

    struct function_decl_stmt {
        token_type return_type;
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

}
