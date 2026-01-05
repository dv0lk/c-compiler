#pragma once
#include <fstream>
#include <string>

// Kinda scuffed, but now we don't need to have a separate runtime file alongside the compiler executable
constexpr std::string_view RUNTIME_SOURCE = R"(#include <stdio.h>
void print_int(int n) { printf("%d\n", n); }

void pause() {
    printf("Press Enter to exit...");
    getchar();
})";

inline void write_runtime(const std::string& name) {
    std::ofstream file(name);
    file << RUNTIME_SOURCE;
}