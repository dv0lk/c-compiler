#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "expressions.hpp"

namespace compiler::ast::stmt {
    class stmt;
    using stmt_ptr = std::shared_ptr<stmt>;

    template <typename T, typename... Args>
    [[nodiscard]] stmt_ptr make_stmt(Args&&... args) {
        return std::make_shared<stmt>(T{std::forward<Args>(args)...});
    }

    struct Return {
        expr::expr_ptr value;
    };

    struct If {
        expr::expr_ptr condition;
        stmt_ptr then_branch;
        std::optional<stmt_ptr> else_branch;
    };

    struct While {
        expr::expr_ptr condition;
        stmt_ptr body;
    };

    struct FunctionParam {
        std::string name;
        TokenType type;
    };

    struct FunctionDecl {
        TokenType return_type;
        std::string function_name;
        std::vector<FunctionParam> params;
        std::optional<stmt_ptr> body;  // nullopt = extern declaration
    };

    struct Block {
        std::vector<stmt_ptr> statements;
    };

    struct Variable {
        std::string name;
        std::optional<expr::expr_ptr> initializer;
    };

    struct ExpressionStmt {
        expr::expr_ptr expression;
    };

    struct ForLoop {
        Variable variable;
        expr::expr_ptr condition;
        stmt_ptr body;
    };

    class stmt {
    private:
        using variant_t = std::variant<Return, If, While, FunctionParam, FunctionDecl, Block, Variable, ExpressionStmt>;
        variant_t data_;

    public:
        template <typename T>
        explicit stmt(T&& value): data_(std::forward<T>(value)) { }

        template <typename T>
        [[nodiscard]] constexpr bool holds() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        template <typename T>
        [[nodiscard]] const T* get_if() const {
            return std::get_if<T>(&data_);
        }

        template <typename Visitor>
        constexpr auto visit(Visitor&& visitor) const {
            return std::visit(std::forward<Visitor>(visitor), data_);
        }
    };
} // namespace compiler::ast::stmt
