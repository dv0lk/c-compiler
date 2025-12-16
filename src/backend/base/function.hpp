#pragma once
#include <span>
#include <string>
#include <vector>
#include "basic_block.hpp"

namespace compiler::base {
    template<typename InstrType>
 class Function {
    private:
        std::string name_;
        std::vector<BasicBlock<InstrType> > basic_blocks_;
        std::vector<std::string> params_; //TODO make actual param type?

    public:
        Function() = default;

        explicit Function(std::string function_name)
            : name_(std::move(function_name)) {
        }

        void add_param(const std::string &param_name) {
            params_.emplace_back(param_name);
        }

        void add_basic_block(BasicBlock<InstrType> &&block) {
            basic_blocks_.emplace_back(std::move(block));
        }

        [[nodiscard]] const std::string &name() const {
            return name_;
        }

        [[nodiscard]] std::span<const std::string> params() const {
            return params_;
        }

        [[nodiscard]] std::vector<BasicBlock<InstrType> >& basic_blocks() {
            return basic_blocks_;
        }

        [[nodiscard]] const std::vector<BasicBlock<InstrType> >& basic_blocks() const {
            return basic_blocks_;
        }

        [[nodiscard]] bool empty() const {
            return basic_blocks_.empty();
        }
    };
}
