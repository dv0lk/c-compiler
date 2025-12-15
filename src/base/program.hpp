#pragma once
#include <span>
#include <string>
#include <vector>
#include "instruction.hpp"

namespace compiler::base {
    template<typename InstrType>
    class BasicBlock {
    private:
        std::vector<InstructionBase<InstrType>> instructions_;

    public:
        BasicBlock() = default;

        void add_instruction(const InstrType &instruction) {
            instructions_.emplace_back(instruction);
        }

        void add_instruction(InstrType &&instruction) {
            instructions_.emplace_back(std::move(instruction));
        }

        [[nodiscard]] std::span<InstrType> instructions() {
            return instructions_;
        }

        [[nodiscard]] std::span<const InstrType> instructions() const {
            return instructions_;
        }

        [[nodiscard]] bool empty() const {
            return instructions_.empty();
        }
    };

    template<typename InstrType>
    class Function {
    private:
        std::string name_;
        std::vector<BasicBlock<InstrType> > basic_blocks_;
        std::vector<std::string> params_; //TODO make actual param type?

    public:
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

        [[nodiscard]] std::span<BasicBlock<InstrType> > basic_blocks() {
            return basic_blocks_;
        }

        [[nodiscard]] std::span<const BasicBlock<InstrType>> basic_blocks() const {
            return basic_blocks_;
        }

        [[nodiscard]] bool empty() const {
            return basic_blocks_.empty();
        }
    };

    template<typename InstrType>
    class Program {
    private:
        std::vector<Function<InstrType> > functions_;

    public:
        Program() = default;

        void add_function(Function<InstrType> &&function) {
            functions_.emplace_back(std::move(function));
        }

        [[nodiscard]] std::span<Function<InstrType>> functions() {
            return functions_;
        }

        [[nodiscard]] std::span<const Function<InstrType>> functions() const {
            return functions_;
        }

        [[nodiscard]] bool empty() const {
            return functions_.empty();
        }

        [[nodiscard]] auto begin() {
            return functions_.begin();
        }

        [[nodiscard]] auto begin() const {
            return functions_.begin();
        }

        [[nodiscard]] auto end() {
            return functions_.end();
        }

        [[nodiscard]] auto end() const {
            return functions_.end();
        }
    };
}
