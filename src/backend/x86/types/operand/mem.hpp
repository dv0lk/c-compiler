#pragma once


namespace compiler::x86 {
    struct Mem {
        int offset;

        Mem() = default;

        explicit Mem(const int offset) : offset(offset) {}

        friend bool operator==(const Mem &lhs, const Mem &rhs) {
            return lhs.offset == rhs.offset;
        }
    };
}