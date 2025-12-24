#pragma once
#include <vector>

#include "transforms/transform.hpp"
#include "ir/ir.hpp"

namespace compiler::transforms {
    template<typename InstrType>
    class CopyPropagation : public Transform<std::vector<BasicBlock<ir::Instruction>> > {
    public:
        CopyPropagation() = default;

        ~CopyPropagation() override = default;

        bool run(std::vector<BasicBlock<ir::Instruction>> &blocks) override {
            copy_map_.clear();

            for (const auto &block: blocks) {
                find_all_copies(block);
            }

            bool changed = false;
            for (auto &block: blocks) {
                changed |= replace_copies(block);
            }
            return changed;
        }

    private:
        // dest -> src
        std::unordered_map<std::string, std::string> copy_map_;

        void find_all_copies(const BasicBlock<ir::Instruction> &block) {
            for (auto &instr: block.instructions()) {
                if (const auto copy = instr.get_if<ir::Copy>()) {
                    auto destination_value = copy->destination;
                    auto source_value = copy->source;

                    if (source_value.is_constant()) continue;

                    auto dest = destination_value.get_variable();
                    auto source = source_value.get_variable();

                    //transitive copies
                    while (copy_map_.contains(source)) {
                        if (source == copy_map_[source]) break;
                        source = copy_map_[source];
                    }

                    copy_map_[dest] = source;
                }
            }
        }

        bool replace_copies(BasicBlock<ir::Instruction> &block) {
            bool changed = false;
            for (auto &instr: block.instructions()) {
                auto new_instr = instr.visit([this](auto &i) { return propagate(i); });
                if (new_instr.has_value()) {
                    instr = std::move(new_instr.value());
                    changed = true;
                }
                invalidate_destination(instr);
            }
            return changed;
        }

        //TODO why does this work???????????????????????????????????
        void invalidate_destination(const ir::Instruction &instr) {
            if (const auto copy = instr.get_if<ir::Copy>()) {
                auto destination_value = copy->destination;
                if (destination_value.is_variable() && copy->source.is_constant()) {
                    const auto& dest = destination_value.get_variable();
                    copy_map_.erase(dest);
                }
            } else if (const auto binary = instr.get_if<ir::Binary>()) {
                auto result_value = binary->result;
                if (result_value.is_variable()) {
                    const auto& dest = result_value.get_variable();
                    copy_map_.erase(dest);
                }
            } else if (const auto unary = instr.get_if<ir::Unary>()) {
                auto result_value = unary->result;
                if (result_value.is_variable()) {
                    const auto& dest = result_value.get_variable();
                    copy_map_.erase(dest);
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
