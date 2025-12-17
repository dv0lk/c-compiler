#pragma once
#include <string>

namespace compiler::x86 {
    struct LabelOperand {
        std::string name;

        LabelOperand() = default;

        explicit LabelOperand(std::string name)
            : name(std::move(name)) {}

        friend bool operator==(const LabelOperand &lhs, const LabelOperand &rhs) {
            return lhs.name == rhs.name;
        }
    };
}
