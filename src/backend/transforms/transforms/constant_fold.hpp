#pragma once
#include <stdexcept>

#include "transforms/transform.hpp"
#include "utils.hpp"
#include "base/program.hpp"


namespace compiler::transforms {
    template<typename InstrType>
    class ConstantFolding : public Transform<std::vector<base::BasicBlock<InstrType>> > {
    public:
        ConstantFolding() = default;

        ~ConstantFolding() override = default;

        bool run(std::vector<base::BasicBlock<InstrType>> &blocks) override {
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
        static bool is_foldable(const ir::instruction &instr) {
            //we only fold binary and unary
            if (!instr.holds<ir::binary, ir::unary>()) {
                return false;
            }

            if (const auto binary = instr.get_if<ir::binary>()) {
                return binary->left.is_constant() && binary->right.is_constant();
            }

            if (const auto unary = instr.get_if<ir::unary>()) {
                return unary->value.is_constant();
            }

            return false;
        }

        static ir::instruction fold(const ir::binary &binary) {
            const int lhs = binary.left.get_constant();
            const int rhs = binary.right.get_constant();
            const int constant = utils::evaluate_binary(binary.op, lhs, rhs);

            return ir::instruction{ir::copy{binary.result, ir::VirtualReg{constant}}};
        }

        static ir::instruction fold(const ir::unary &binary) {
            const int value = binary.value.get_constant();
            const int constant = utils::evaluate_unary(value, binary.op);

            return ir::instruction{ir::copy{binary.result, ir::VirtualReg{constant}}};
        }

        template<typename T>
        static ir::instruction fold(const T &instr) {
            //should be unreachable
            throw std::runtime_error("Unsupported instruction for constant folding");
        }
    };
}
