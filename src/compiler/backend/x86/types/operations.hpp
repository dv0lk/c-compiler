#pragma once
#include <utility>

#include "operand.hpp"
#include "condition_code.hpp"

namespace compiler::x86 {
    struct Mov {
        Operand destination;
        Operand source;

        Mov() = default;

        Mov(const Operand &destination, const Operand &source)
            : destination(destination), source(source) {
        }

        Mov(Operand &&destination, Operand &&source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct Ret {
        Ret() = default;
    };

    struct Neg {
        Operand value;

        Neg() = default;

        explicit Neg(Operand value)
            : value(std::move(value)) {
        }
    };

    struct Not {
        Operand value;

        Not() = default;

        explicit Not(Operand value)
            : value(std::move(value)) {
        }
    };

    struct Add {
        Operand destination;
        Operand source;

        Add() = default;

        Add(Operand destination, Operand source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct Sub {
        Operand destination;
        Operand source;

        Sub() = default;

        Sub(Operand destination, Operand source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct Imul {
        Operand destination;
        Operand source;

        Imul() = default;

        Imul(Operand destination, Operand source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct cdq {
        cdq() = default;
    };

    struct Idiv {
        Operand value;

        Idiv() = default;

        explicit Idiv(Operand value)
            : value(std::move(value)) {
        }
    };

    struct Cmp {
        Operand destination;
        Operand source;

        Cmp() = default;

        Cmp(Operand destination, Operand source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct Label {
        LabelOp target;

        Label() = default;

        explicit Label(LabelOp target)
            : target(std::move(target)) {
        }

        explicit Label(std::string target) : target(LabelOp(std::move(target))) {
        }
    };

    struct Jmp {
        LabelOp target;

        Jmp() = default;

        explicit Jmp(LabelOp target)
            : target(std::move(target)) {
        }
    };

    struct JmpCC {
        CC condition = CC::Empty;
        LabelOp target;

        JmpCC() = default;

        JmpCC(const CC condition, LabelOp target)
            : condition(condition), target(std::move(target)) {
        }
    };

    struct SetCC {
        CC condition = CC::Empty;
        Operand value;

        SetCC() = default;

        SetCC(const CC condition, Operand value)
            : condition(condition), value(std::move(value)) {
        }
    };

    struct Push {
        Operand value;

        Push() = default;

        explicit Push(Operand value)
            : value(std::move(value)) {
        }
    };

    struct Pop {
        Operand value;

        Pop() = default;

        explicit Pop(Operand value)
            : value(std::move(value)) {
        }
    };

    struct Call {
        Operand target;

        Call() = default;

        explicit Call(Operand target)
            : target(std::move(target)) {
        }
    };
}
