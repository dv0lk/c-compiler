#include "parser.h"

#include <stdexcept>

namespace compiler::parser {
    std::vector<ast::stmt_ptr> parser::parse_ast(std::vector<token> tokens) {
        this->tokens = std::move(tokens);
        while (!is_end()) {
            this->statements.emplace_back(parse_declaration_statement());
        }
        return this->statements;
    }

    bool parser::is_end() const {
        return current_position >= tokens.size();
    }

    token parser::advance() {
        current_position++;
        return previous();
    }

    token parser::peek() const {
        return tokens[current_position];
    }

    std::optional<token> parser::peek_next() const {
        if (is_end()) {
            return {};
        }
        return tokens[current_position + 1];
    }

    token parser::previous() const {
        return tokens[current_position - 1];
    }

    bool parser::check(const token_type type) const {
        if (is_end()) {
            return false;
        }
        return peek().get_type() == type;
    }

    token parser::consume(const token_type type, const std::string& error_message) {
        if (check(type)) {
            return advance();
        }
        throw std::runtime_error(error_message);
    }

    ast::expr_ptr parser::parse_expression() {
        return parse_assignment_expr();
    }

    ast::expr_ptr parser::parse_assignment_expr() {
        auto expression = parse_logical_or_expr();
        if (match(token_type::Equal)) {
            auto value = parse_assignment_expr();

            if (const auto variable = std::get_if<ast::variable_expr>(&*expression)) {
                auto name = variable->name;
                return ast::make_expr<ast::assignment_expr>(name, value);
            }
        }
        return expression;
    }

    ast::expr_ptr parser::parse_logical_or_expr() {
        auto expression = parse_logical_and_expr();
        while (match(token_type::LogicalOr)) {
            auto op = previous().get_type();
            auto right = parse_logical_and_expr();
            expression = ast::make_expr<ast::logical_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_logical_and_expr() {
        auto expression = parse_equality_expr();

        while (match(token_type::LogicalAnd)) {
            auto op = previous().get_type();
            auto right = parse_equality_expr();
            expression = ast::make_expr<ast::logical_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_equality_expr() {
        auto expression = parse_comparison_expr();
        while (match(token_type::NotEqual, token_type::EqualEqual)) {
            auto op = previous().get_type();
            auto right = parse_comparison_expr();
            expression = ast::make_expr<ast::binary_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_comparison_expr() {
        auto expression = parse_additive_expr();
        while (match(token_type::Less, token_type::LessEqual, token_type::Greater, token_type::GreaterEqual)) {
            auto op = previous().get_type();
            auto right = parse_additive_expr();
            expression = ast::make_expr<ast::binary_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_additive_expr() {
        auto expression = parse_multiplicative_expr();
        while (match(token_type::Plus, token_type::Minus)) {
            auto op = previous().get_type();
            auto right = parse_multiplicative_expr();
            expression = ast::make_expr<ast::binary_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_multiplicative_expr() {
        auto expression = parse_unary_expr();
        while (match(token_type::Star, token_type::Slash)) {
            auto op = previous().get_type();
            auto right = parse_unary_expr();
            expression = ast::make_expr<ast::binary_expr>(expression, op, right);
        }
        return expression;
    }

    ast::expr_ptr parser::parse_unary_expr() {
        if (match(token_type::Tilde, token_type::Minus, token_type::Not)) {
            auto op = previous().get_type();
            auto right = parse_unary_expr();
            return ast::make_expr<ast::unary_expr>(op, right);
        }
        return parse_primary_expr();
    }

    ast::expr_ptr parser::parse_primary_expr() {
        if (match(token_type::IntLiteral, token_type::StringLiteral, token_type::DoubleLiteral)) {
            return ast::make_expr<ast::literal_expr>(*previous().get_literal());
        }

        if (match(token_type::LeftParen)) {
            auto expr = parse_expression();
            consume(token_type::RightParen, "Expected ')' after expression");
            return ast::make_expr<ast::grouping_expr>(expr);
        }

        if (match(token_type::Identifier)) {
            const std::string name = previous().get_lexeme();
            if (match(token_type::LeftParen))
                return parse_call_expr(name);
            return ast::make_expr<ast::variable_expr>(name);
        }

        throw std::runtime_error("Encounter Unknown expression while parsing");
    }

    ast::expr_ptr parser::parse_call_expr(const std::string& name) {
        std::vector<ast::expr_ptr> arguments;

        if (!check(token_type::RightParen)) {
            do {
                arguments.push_back(parse_expression());
            } while (match(token_type::Comma));
        }

        consume(token_type::RightParen, "Expected ')' after arguments");
        return ast::make_expr<ast::call_expr>(name, arguments);
    }

    ast::stmt_ptr parser::parse_statement() {
        if (match(token_type::If)) {
            return parse_if_statement();
        }

        if (match(token_type::While)) {
            return parse_while_statement();
        }

        if (match(token_type::LeftBrace)) {
            return parse_block_statement();
        }

        if (match(token_type::Return)) {
            return parse_return_statement();
        }

        return parse_expression_statement();
    }

    ast::stmt_ptr parser::parse_declaration_statement() {
        if (match(token_type::Int, token_type::Char, token_type::Void, token_type::Double)) {
            auto next_token = peek_next();

            if (next_token.has_value()) {
                if (next_token->get_type() == token_type::Equal || next_token->get_type() == token_type::Semicolon) {
                    return parse_variable_declaration_statement();
                }

                if (next_token->get_type() == token_type::LeftParen) {
                    return parse_function_declaration_statement();
                }
            }
            throw std::runtime_error("Expected variable declaration or function declaration");
        }
        return parse_statement();
    }

    //todo add support for multiple types
    ast::stmt_ptr parser::parse_variable_declaration_statement() {
        std::string variable_name = consume(token_type::Identifier, "Expected identifier after type").get_lexeme();

        std::optional<ast::expr_ptr> initializer;
        if (match(token_type::Equal)) {
            initializer = parse_expression();
        }
        consume(token_type::Semicolon, "Expected ';' after variable declaration");

        return ast::make_stmt<ast::variable_stmt>(variable_name, initializer);
    }

    ast::stmt_ptr parser::parse_function_declaration_statement() {
        auto return_type = previous().get_type();
        auto function_name = consume(token_type::Identifier, "Expected function name after type").get_lexeme();
        consume(token_type::LeftParen, "Expected '(' after function name");

        std::vector<ast::function_param_stmt> params;
        if (!check(token_type::RightParen)) {
            do {
                //todo currently support only int
                auto param_type = consume(token_type::Int, "Expected parameter type").get_type();
                auto param_name = consume(token_type::Identifier, "Expected parameter name").get_lexeme();
                params.emplace_back(param_name, param_type);
            } while (match(token_type::Comma) && !is_end());
        }

        consume(token_type::RightParen, "Expected ')' after parameters");
        consume(token_type::LeftBrace, "Expected '{' before function body");

        auto body = parse_block_statement();

        return ast::make_stmt<ast::function_decl_stmt>(return_type, function_name, params, body);
    }

    ast::stmt_ptr parser::parse_if_statement() {
        consume(token_type::LeftParen, "Expected '(' after if");
        auto condition = parse_expression();
        consume(token_type::RightParen, "Expected ')' after if condition");
        auto then_branch = parse_statement();

        std::optional<ast::stmt_ptr> else_branch;
        if (match(token_type::Else)) {
            else_branch = parse_statement();
        }

        return ast::make_stmt<ast::if_stmt>(condition, then_branch, else_branch);
    }

    ast::stmt_ptr parser::parse_block_statement() {
        std::vector<ast::stmt_ptr> statements;

        while (!check(token_type::RightBrace) && !is_end()) {
            statements.emplace_back(parse_declaration_statement());
        }
        consume(token_type::RightBrace, "Expected '}' after block");

        return ast::make_stmt<ast::block_stmt>(statements);
    }

    ast::stmt_ptr parser::parse_while_statement() {
        consume(token_type::LeftParen, "Expected '(' after while");
        auto condition = parse_expression();
        consume(token_type::RightParen, "Expected ')' after while condition");
        auto body = parse_statement();
        return ast::make_stmt<ast::while_stmt>(condition, body);
    }

    ast::stmt_ptr parser::parse_expression_statement() {
        auto expression = ast::make_stmt<ast::expression_stmt>(parse_expression());
        consume(token_type::Semicolon, "Expected ';' after expression");
        return expression;
    }

    ast::stmt_ptr parser::parse_return_statement() {
        auto expression = parse_expression();
        auto return_stmt = ast::make_stmt<ast::return_stmt>(expression);
        consume(token_type::Semicolon, "Expected ';' after return statement");
        return return_stmt;
    }
}
