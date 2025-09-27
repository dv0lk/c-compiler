#include "compiler/compiler.hpp"
#include "util/files.h"
#include <filesystem>

int main() {
    const auto project_path = std::filesystem::current_path().parent_path();
    const std::string path = (project_path / "tests" / "test.c").string();


    const auto file = files::read_file(path);
    if (!file.has_value())
        throw std::runtime_error("Failed to read file.");

    const std::string source = {file->begin(), file->end()};

    compiler::Compiler compiler;
    compiler.compile(source);
}
