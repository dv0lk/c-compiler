#pragma once
#include <string>
#include <vector>
#include "basic_block.hpp"

namespace compiler {
    template<typename InstrType>
    class Function {
    public:
        using block_t = BasicBlock<InstrType>;

        Function() = default;

        explicit Function(std::string function_name)
            : name_(std::move(function_name)) {
        }

        void add_param(std::string param_name) {
            params_.emplace_back(std::move(param_name));
        }

        void add_bb(block_t block) {
            basic_blocks_.emplace_back(std::move(block));
        }

        [[nodiscard]] const std::string &name() const { return name_; }

        [[nodiscard]] std::vector<std::string> params() const { return params_; }

        [[nodiscard]] std::vector<block_t> &basic_blocks() { return basic_blocks_; }
        [[nodiscard]] const std::vector<block_t> &basic_blocks() const { return basic_blocks_; }

        [[nodiscard]] bool empty() const { return basic_blocks_.empty(); }

    private:
        std::string name_;
        std::vector<block_t> basic_blocks_;
        std::vector<std::string> params_; //TODO make actual param type?
    };
}
