#pragma once
#include <stdexcept>

#include "lexer/token.h"

namespace compiler::transforms::utils {
    static int evaluate_binary(const token_t op, const int left, const int right) {
        switch (op) {
            default:
                throw std::runtime_error("Unknown binary operation for constant folding");
            case token_t::Plus:
                return left + right;
            case token_t::Minus:
                return left - right;
            case token_t::Star:
                return left * right;
            case token_t::Slash:
                return left / right;
            case token_t::NotEqual:
                return left != right ? 1 : 0;
            case token_t::EqualEqual:
                return left == right ? 1 : 0;
            case token_t::Less:
                return left < right ? 1 : 0;
            case token_t::LessEqual:
                return left <= right ? 1 : 0;
            case token_t::Greater:
                return left > right ? 1 : 0;
            case token_t::GreaterEqual:
                return left >= right ? 1 : 0;
        }
    }

    static int evaluate_unary(const int value, const token_t op) {
        switch (op) {
            default:
                throw std::runtime_error("Unknown unary operation for constant folding");
            case token_t::Minus:
                return -value;
            case token_t::Tilde:
                return ~value;
            case token_t::Not:
                return !value ? 1 : 0;
        }
    }
}
