#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "lexer/token.h"

#include "ir_value.hpp"
struct ir_return {
    ir_value value;
};

struct ir_binary {
    token_t op;
    ir_value left;
    ir_value right;
    ir_value result;
};

struct ir_unary {
    token_t op;
    ir_value value;
    ir_value result;
};

struct ir_copy {
    ir_value destination;
    ir_value source;

    friend bool operator==(const ir_copy& lhs, const ir_copy& rhs) {
        return lhs.destination == rhs.destination
               && lhs.source == rhs.source;
    }
};

struct ir_label {
    std::string name;
};

struct ir_jump {
    ir_label label;
};

struct ir_jump_if_zero {
    ir_value condition;
    ir_label label;
};

struct ir_jump_if_not_zero {
    ir_value condition;
    ir_label label;
};

struct ir_call {
    std::string function_name;
    std::vector<ir_value> arguments;
    ir_value destination;
};