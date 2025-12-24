#pragma once
#include "ir/types/operand.hpp"
#include "frontend/lexer/token.hpp"

namespace compiler::ir {

    struct Return {
        Operand value;

        bool operator==(const Return &) const = default;
    };

    struct Binary {
        TokenType op;
        Operand left;
        Operand right;
        Operand result;

        bool operator==(const Binary &) const = default;

    };

    struct Unary {
        TokenType op;
        Operand value;
        Operand result;

        bool operator==(const Unary &) const = default;
    };

    struct Copy {
        Operand destination;
        Operand source;

        bool operator==(const Copy &) const = default;
    };

    struct Label {
        std::string name;

        bool operator==(const Label &) const = default;
    };

    struct Jump {
        Label target;

        bool operator==(const Jump &) const = default;
    };

    struct JumpIfZero {
        Operand condition;
        Label target_label;

        bool operator==(const JumpIfZero &) const = default;
    };

    struct JumpIfNotZero {
        Operand condition;
        Label target_label;

        bool operator==(const JumpIfNotZero &) const = default;
    };

    struct FunctionCall {
        std::string function_name;
        std::vector<Operand> arguments;
        Operand destination;

        bool operator==(const FunctionCall &) const = default;
    };
}
