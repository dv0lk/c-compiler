#pragma once
#include <string>
#include <vector>

#include "basic_block_t.hpp"


//TODO we should probably make basic blocks and function/program be instruction independent
namespace compiler::ir {

    struct function_t {
        std::string name;
        std::vector<std::string> params;
        std::vector<basic_block_t> blocks;

        function_t() = default;
        function_t(std::string name, std::vector<std::string> params = {}, std::vector<basic_block_t> blocks = {})
            : name(std::move(name)), params(std::move(params)), blocks(std::move(blocks)) {}

        [[nodiscard]] std::span<basic_block_t> basic_blocks() {
            return blocks;
        }

        [[nodiscard]] std::span<const basic_block_t> basic_blocks() const {
            return blocks;
        }

        [[nodiscard]] bool empty() const {
            return blocks.empty();
        }

        void add_bb(basic_block_t bb) {
            blocks.emplace_back(std::move(bb));
        }
    };

    struct program_t {
        std::vector<function_t> functions;

        program_t() = default;

        [[nodiscard]] bool empty() const {
            return functions.empty();
        }

        void add_function(function_t func) {
            functions.emplace_back(std::move(func));
        }

        [[nodiscard]] auto begin() {
            return functions.begin();
        }

        [[nodiscard]] auto begin() const {
            return functions.begin();
        }

        [[nodiscard]] auto end() {
            return functions.end();
        }

        [[nodiscard]] auto end() const {
            return functions.end();
        }

    };

}