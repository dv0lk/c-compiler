#pragma once

struct Imm {
    int value;

    explicit Imm(const int value)
        : value(value) {}

    friend bool operator==(const Imm &lhs, const Imm &rhs) {
        return lhs.value == rhs.value;
    }
};