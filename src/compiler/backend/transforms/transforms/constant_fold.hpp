#pragma once
#include <stdexcept>

#include "transforms/transform.hpp"
#include "utils.hpp"

namespace compiler::transforms {
    template<typename InstrType>
    class ConstantFolding : public Transform<std::vector<BasicBlock<ir::Instruction> > > {
    public:
        ConstantFolding() = default;

        ~ConstantFolding() override = default;

        bool run(std::vector<BasicBlock<ir::Instruction> > &blocks) override {
            bool changed = false;
            for (auto &block: blocks) {
                for (auto &instr: block.instructions()) {
                    if (is_foldable(instr)) {
                        auto folded = instr.visit([](auto &i) { return fold(i); });
                        instr = std::move(folded);
                        changed = true;
                    }
                }
            }

            return changed;
        }

    private:
        static bool is_foldable(const ir::Instruction &instr) {
            //we only fold binary and unary
            if (!instr.holds<ir::Binary, ir::Unary>()) {
                return false;
            }

            if (const auto binary = instr.get_if<ir::Binary>()) {
                return binary->left.is_constant() && binary->right.is_constant();
            }

            if (const auto unary = instr.get_if<ir::Unary>()) {
                return unary->value.is_constant();
            }

            return false;
        }

        static ir::Instruction fold(const ir::Binary &binary) {
            const int lhs = binary.left.get_constant();
            const int rhs = binary.right.get_constant();
            const int constant = utils::evaluate_binary(binary.op, lhs, rhs);

            return ir::Instruction{ir::Copy{binary.result, ir::Operand{constant}}};
        }

        static ir::Instruction fold(const ir::Unary &binary) {
            const int value = binary.value.get_constant();
            const int constant = utils::evaluate_unary(value, binary.op);

            return ir::Instruction{ir::Copy{binary.result, ir::Operand{constant}}};
        }

        template<typename T>
        static ir::Instruction fold(const T &) {
            //should be unreachable
            throw std::runtime_error("Unsupported instruction for constant folding");
        }
    };
}
