#pragma once
#include <vector>

#include "transforms/transform.hpp"
#include "ir/ir.hpp"
#include "analysis/traits/traits.hpp"

namespace compiler::transforms {
    template<typename InstrType>
    class CopyPropagation : public Transform<InstrType> {
    public:
        CopyPropagation() = default;

        ~CopyPropagation() override = default;

        bool run(std::vector<InstrType> &instructions) override {
            copy_map_.clear();
            bool changed = false;

            for (auto &instr : instructions) {
                // Reset map at labels (conservative for control flow)
                if (InstructionTrait<InstrType>::is_label(instr)) {
                    copy_map_.clear();
                    continue;
                }

                auto new_instr = instr.visit([this](auto &i) { return propagate(i); });
                if (new_instr.has_value()) {
                    instr = std::move(new_instr.value());
                    changed = true;
                }

                update_copy_map(instr);
            }

            return changed;
        }

    private:
        // dest -> src
        std::unordered_map<std::string, std::string> copy_map_;

        void invalidate_variable(const std::string &var) {
            copy_map_.erase(var);
            std::erase_if(copy_map_, [&var](const auto &pair) {
                return pair.second == var;
            });
        }

        void update_copy_map(const ir::Instruction &instr) {
            if (const auto copy = instr.get_if<ir::Copy>()) {
                auto dest = copy->destination.get_variable();
                invalidate_variable(dest);

                if (copy->source.is_variable()) {
                    auto src = copy->source.get_variable();
                    while (copy_map_.contains(src)) {
                        if (src == copy_map_[src]) break;
                        src = copy_map_[src];
                    }
                    copy_map_[dest] = src;
                }
            } else if (const auto binary = instr.get_if<ir::Binary>()) {
                if (binary->result.is_variable()) {
                    invalidate_variable(binary->result.get_variable());
                }
            } else if (const auto unary = instr.get_if<ir::Unary>()) {
                if (unary->result.is_variable()) {
                    invalidate_variable(unary->result.get_variable());
                }
            }
        }

        std::optional<ir::Operand> replace_operand(ir::Operand &operand) {
            if (operand.is_constant() || !copy_map_.contains(operand.get_variable())) {
                return std::nullopt;
            }

            return ir::Operand{copy_map_[operand.get_variable()]};
        }

        std::optional<ir::Instruction> propagate(const ir::Copy &copy) {
            ir::Operand source = copy.source;

            const auto new_source = replace_operand(source);

            if (new_source.has_value()) {
                return ir::Instruction{ir::Copy{copy.destination, new_source.value()}};
            }

            return std::nullopt;
        }

        std::optional<ir::Instruction> propagate(const ir::Binary &binary) {
            ir::Operand left = binary.left;
            ir::Operand right = binary.right;

            const auto new_left = replace_operand(left);
            const auto new_right = replace_operand(right);

            if (new_left.has_value() || new_right.has_value()) {
                return ir::Instruction{ir::Binary{binary.op, new_left.value_or(left), new_right.value_or(right), binary.result}};
            }

            return std::nullopt;
        }


        std::optional<ir::Instruction> propagate(const ir::Unary &unary) {
            ir::Operand operand = unary.value;

            const auto new_operand = replace_operand(operand);

            if (new_operand.has_value()) {
                return ir::Instruction(ir::Unary(unary.op, new_operand.value(), unary.result));
            }

            return std::nullopt;
        }

        std::optional<ir::Instruction> propagate(const ir::Return &ret) {
            ir::Operand value = ret.value;

            const auto new_value = replace_operand(value);

            if (new_value.has_value()) {
                return ir::Instruction{ir::Return{new_value.value()}};
            }

            return std::nullopt;
        }

        template<typename T>
        std::optional<ir::Instruction> propagate(const T&) {
            return std::nullopt;
        }
    };
}
