#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>

#include "argparse/argparse.hpp"
#include "util/files.hpp"

class Config {
public:
    std::string input_file;

    bool print_ast = false;

    bool print_ir_no_opt = false;
    bool print_ir = false;

    bool print_x86_no_opt = false;
    bool print_x86 = false;

    void init(int argc, char* argv[]) {
        parser_.add_description("A simple C compiler");
        parser_.add_epilog("Example: compiler input.c --ast --ir-before-opt");

        parser_.add_argument("input")
            .help("input source file (.c)")
            .required();

        parser_.add_argument("--ast")
            .help("print the AST")
            .flag();

        parser_.add_argument("--ir")
            .help("print IR (after optimizations)")
            .flag();

        parser_.add_argument("--ir-no-opt")
            .help("print IR before optimizations")
            .flag();

        parser_.add_argument("--x86")
            .help("print x86 assembly (after optimizations)")
            .flag();

        parser_.add_argument("--x86-no-opt")
            .help("print x86 assembly before optimizations")
            .flag();

        // parser_.add_argument("-o", "--output")
        //     .help("output file path")
        //     .default_value(std::string("a.out"));

        try {
            parser_.parse_args(argc, argv);
        } catch (const std::exception& err) {
            std::cerr << err.what() << std::endl;
            std::cerr << parser_;
            std::exit(1);
        }

        input_file_path = parser_.get<std::string>("input");
        validate_input_file();

        print_ast = parser_.get<bool>("--ast");

        print_ir = parser_.get<bool>("--ir");
        print_ir_no_opt = parser_.get<bool>("--ir-no-opt");

        print_x86 = parser_.get<bool>("--x86");
        print_x86_no_opt = parser_.get<bool>("--x86-no-opt");

        // output_file = parser_.get<std::string>("--output");
    }

    [[nodiscard]] const std::string& get_output_file() const {
        return output_file;
    }

private:
    argparse::ArgumentParser parser_{"compiler"};
    std::string output_file;
    std::string input_file_path;

    void validate_input_file() {
        if (!input_file_path.ends_with(".c")) {
            throw std::runtime_error("Input file must have .c extension: " + input_file_path);
        }

        if (!std::filesystem::exists(input_file_path)) {
            throw std::runtime_error("Input file does not exist");
        }

        const auto file = files::read_file(input_file_path);
        if (!file.has_value())
            throw std::runtime_error("Failed to read file");

        input_file = {file->begin(), file->end()};
    }
};
