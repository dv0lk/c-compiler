#pragma once
#include <format>
#include <string>
#include <utility>
#include <variant>

namespace compiler::x86 {

    struct Register {
        Register() = default;

        explicit Register(const std::string& name)
            : name(name) {}

        std::string name;

        [[nodiscard]] std::string to_str() const {
            return name;
        }

        friend bool operator==(const Register& lhs, const Register& rhs) {
            return lhs.name == rhs.name;
        }

    };

    //TODO hmm, think about this one
    namespace registers {
        inline const Register RAX{"rax"};
        inline const Register RBX{"rbx"};
        inline const Register RCX{"rcx"};
        inline const Register RDX{"rdx"};
        inline const Register RDI{"rdi"};
        inline const Register RSI{"rsi"};
        inline const Register RBP{"rbp"};
        inline const Register RSP{"rsp"};
        inline const Register R8{"r8"};
        inline const Register R9{"r9"};
        inline const Register R10{"r10"};
        inline const Register R11{"r11"};
        inline const Register R12{"r12"};
        inline const Register R13{"r13"};
        inline const Register R14{"r14"};
        inline const Register R15{"r15"};
    }

    struct Imm {
        int value;

        explicit Imm(const int value)
            : value(value) {}

        [[nodiscard]] std::string to_str() const {
            return std::to_string(value);
        }

        friend bool operator==(const Imm& lhs, const Imm& rhs) {
            return lhs.value == rhs.value;
        }

    };

    struct Label {
        std::string name;

        explicit Label(std::string name)
            : name(std::move(name)) {}

        [[nodiscard]] std::string to_str() const {
            return name;
        }

        friend bool operator==(const Label& lhs, const Label& rhs) {
            return lhs.name == rhs.name;
        }

    };

    struct Mem {
        int offset;

        explicit Mem(int offset)
            : offset(offset) {}

        [[nodiscard]] std::string to_str() const {
            if (offset < 0) {
                return std::format("[rbp{:d}]", offset);
            } else {
                return std::format("[rbp+{:d}]", offset);
            }
        }

        friend bool operator==(const Mem& lhs, const Mem& rhs) {
            return lhs.offset == rhs.offset;
        }

    };

    //TODO temp thing
    struct PseudoRegister {
        std::string name;

        explicit PseudoRegister(const std::string& name)
            : name(name) {}

        [[nodiscard]] std::string to_str() const {
            return name;
        }

        friend bool operator==(const PseudoRegister& lhs, const PseudoRegister& rhs) {
            return lhs.name == rhs.name;
        }

    };

    using Operand = std::variant<Register, Imm, Label, Mem, PseudoRegister>;
    // TODO Maybe make this a class that will hold std::variant<register, immediate, label>, similar how it's done in zasm
    //  https://github.com/zyantific/zasm/blob/c239a78b51c1b0060296193174d78b802f02a618/zasm/include/zasm/base/operand.hpp#L75
    //  overall something similar can be done in a lot of places

    //TODO I dont like this
    inline std::string operand_to_str(const Operand& operand) {
        return std::visit([](const auto& op) {
            return op.to_str();
        }, operand);
    }

    enum class CC {
        Equal, //e
        NotEqual, //ne
        Less, //l
        LessOrEqual, //le
        Greater, //g
        GreaterOrEqual, //ge
        Below, //b
        BelowOrEqual, //be
        Above, //a
        AboveOrEqual, //ae
        Overflow, //o
        NoOverflow, //no
        Sign, //s
        NotSign, //ns
        ParityEven, // p
        ParityOdd // np
    };

    inline std::string cc_to_str(const CC cc) {
        switch (cc) {
        case CC::Equal:
            return "e";
        case CC::NotEqual:
            return "ne";
        case CC::Less:
            return "l";
        case CC::LessOrEqual:
            return "le";
        case CC::Greater:
            return "g";
        case CC::GreaterOrEqual:
            return "ge";
        case CC::Below:
            return "b";
        case CC::BelowOrEqual:
            return "be";
        case CC::Above:
            return "a";
        case CC::AboveOrEqual:
            return "ae";
        case CC::Overflow:
            return "o";
        case CC::NoOverflow:
            return "no";
        case CC::Sign:
            return "s";
        case CC::NotSign:
            return "ns";
        case CC::ParityEven:
            return "p";
        case CC::ParityOdd:
            return "np";
        default:
            return "UNKNOWN";
        }
    }


    struct mov {
        Operand source;
        Operand destination;

        [[nodiscard]] std::string emit() const {
            return std::format("mov {}, {}", operand_to_str(source), operand_to_str(destination));
        }
    };

    struct ret {
        [[nodiscard]] static std::string emit() {
            return "ret";
        }
    };

    struct neg {
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("neg {}", operand_to_str(value));
        }
    };

    struct not_ {
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("not {}", operand_to_str(value));
        }
    };

    struct add {
        Operand source;
        Operand destination;

        [[nodiscard]] std::string emit() const {
            return std::format("add {}, {}", operand_to_str(source), operand_to_str(destination));
        }
    };

    struct sub {
        Operand source;
        Operand destination;

        [[nodiscard]] std::string emit() const {
            return std::format("sub {}, {}", operand_to_str(source), operand_to_str(destination));
        }
    };

    struct imul {
        Operand source;
        Operand destination;

        [[nodiscard]] std::string emit() const {
            return std::format("imul {}, {}", operand_to_str(source), operand_to_str(destination));
        }
    };

    struct cdq {
        [[nodiscard]] static std::string emit() {
            return std::format("cdq");
        }
    };

    struct idiv {
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("idiv {}", operand_to_str(value));
        }
    };

    struct cmp {
        Operand source;
        Operand destination;

        [[nodiscard]] std::string emit() const {
            return std::format("cmp {}, {}", operand_to_str(source), operand_to_str(destination));
        }
    };

    struct label {
        Label name;

        [[nodiscard]] std::string emit() const {
            return std::format("{}:", operand_to_str(name));
        }
    };

    struct jmp {
        Label target;

        [[nodiscard]] std::string emit() const {
            return std::format("jmp {}", operand_to_str(target));
        }
    };

    //condition code jmp
    struct jmp_cc {
        CC cc;
        Label target;

        [[nodiscard]] std::string emit() const {
            return std::format("j{} {}", cc_to_str(cc), operand_to_str(target));
        }
    };

    struct set_cc {
        CC cc;
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("set{} {}", cc_to_str(cc), operand_to_str(value));
        }
    };

    struct push {
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("push {}", operand_to_str(value));
        }
    };

    struct pop {
        Operand value;

        [[nodiscard]] std::string emit() const {
            return std::format("pop {}", operand_to_str(value));
        }
    };

    using instruction = std::variant<mov, ret, neg, not_, add, sub, imul, cdq, idiv, cmp, label, jmp, jmp_cc, set_cc, push, pop>;
}
