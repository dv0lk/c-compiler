#pragma once
#include "virt_reg.hpp"
#include "lexer/token.h"

namespace compiler::ir {
    struct return_ {
        VirtualReg value;

        bool operator==(const return_ &) const = default;
    };

    struct binary {
        token_t op;
        VirtualReg left;
        VirtualReg right;
        VirtualReg result;

        bool operator==(const binary &) const = default;

    };

    struct unary {
        token_t op;
        VirtualReg value;
        VirtualReg result;

        bool operator==(const unary &) const = default;
    };

    struct copy {
        VirtualReg destination;
        VirtualReg source;

        bool operator==(const copy &) const = default;
    };

    struct label {
        std::string name;

        bool operator==(const label &) const = default;
    };

    struct jump {
        label target;

        bool operator==(const jump &) const = default;
    };

    struct jump_if_zero {
        VirtualReg condition;
        label target_label;

        bool operator==(const jump_if_zero &) const = default;
    };

    struct jump_if_not_zero {
        VirtualReg condition;
        label target_label;

        bool operator==(const jump_if_not_zero &) const = default;
    };

    struct func_call {
        std::string function_name;
        std::vector<VirtualReg> arguments;
        VirtualReg destination;

        bool operator==(const func_call &) const = default;
    };
}
