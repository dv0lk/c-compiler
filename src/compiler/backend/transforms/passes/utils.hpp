#pragma once
#include <stdexcept>

#include "frontend/lexer/token.hpp"

namespace compiler::transforms::utils {
    static int evaluate_binary(const TokenType op, const int left, const int right) {
        switch (op) {
        case TokenType::Plus:
            return left + right;
        case TokenType::Minus:
            return left - right;
        case TokenType::Star:
            return left * right;
        case TokenType::Slash:
            return left / right;
        case TokenType::NotEqual:
            return left != right ? 1 : 0;
        case TokenType::EqualEqual:
            return left == right ? 1 : 0;
        case TokenType::Less:
            return left < right ? 1 : 0;
        case TokenType::LessEqual:
            return left <= right ? 1 : 0;
        case TokenType::Greater:
            return left > right ? 1 : 0;
        case TokenType::GreaterEqual:
            return left >= right ? 1 : 0;
        default:
            throw std::runtime_error("Unknown binary operation for constant folding");
        }
    }

    static int evaluate_unary(const int value, const TokenType op) {
        switch (op) {
        case TokenType::Minus:
            return -value;
        case TokenType::Tilde:
            return ~value;
        case TokenType::Not:
            return !value ? 1 : 0;
        default:
            throw std::runtime_error("Unknown unary operation for constant folding");
        }
    }
} // namespace compiler::transforms::utils
