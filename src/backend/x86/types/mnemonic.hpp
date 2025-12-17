#pragma once
#include <utility>

#include "operand/operand.hpp"
#include "condition_code.hpp"
namespace compiler::x86 {
    struct Mov {
        Operand destination;
        Operand source;

        Mov() = default;

        Mov(const Operand& destination, const Operand& source)
            : destination(destination), source(source) {
        }

        Mov(Operand&& destination, Operand&& source)
            : destination(std::move(destination)), source(std::move(source)) {
        }
    };

    struct Ret {
        Ret() = default;
    };

    struct Neg {
        Operand value;

        Neg() = default;

        explicit Neg(const Operand& value)
            : value(value) {
        }
    };

    struct Not {
        Operand value;

        Not() = default;

        explicit Not(const Operand& value)
            : value(value) {
        }
    };

    struct Add {
        Operand destination;
        Operand source;

        Add() = default;

        Add(const Operand& destination, const Operand& source)
            : destination(destination), source(source) {
        }
    };

    struct Sub {
        Operand destination;
        Operand source;

        Sub() = default;

        Sub(const Operand& destination, const Operand& source)
            : destination(destination), source(source) {
        }
    };

    struct Imul {
        Operand destination;
        Operand source;

        Imul() = default;

        Imul(const Operand& destination, const Operand& source)
            : destination(destination), source(source) {
        }
    };

    struct cdq {
        cdq() = default;
    };

    struct Idiv {
        Operand value;

        Idiv() = default;

        explicit Idiv(const Operand& value)
            : value(value) {
        }
    };

    struct Cmp {
        Operand destination;
        Operand source;

        Cmp() = default;

        Cmp(const Operand& destination, const Operand& source)
            : destination(destination), source(source) {
        }
    };

    struct Label {
        LabelOperand target;

        Label() = default;

        explicit Label(const LabelOperand& target)
            : target(target) {
        }
    };

    struct Jmp {
        LabelOperand target;

        Jmp() = default;

        explicit Jmp(const LabelOperand& target)
            : target(target) {
        }
    };

    struct JmpCC {
        CC condition = CC::Empty;
        LabelOperand target;

        JmpCC() = default;

        JmpCC(const CC condition, LabelOperand target)
            : condition(condition), target(std::move(target)) {
        }
    };

    struct SetCC {
        CC condition = CC::Empty;
        Operand value;

        SetCC() = default;

        SetCC(CC condition, const Operand& value)
            : condition(condition), value(value) {
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
