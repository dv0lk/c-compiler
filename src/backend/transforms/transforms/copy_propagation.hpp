#pragma once
#include <vector>

#include "transforms/transform.hpp"
#include "ir/ir.hpp"

namespace compiler::transforms {
    template<typename InstrType>
    class CopyPropagation : public Transform<std::vector<base::BasicBlock<InstrType>> > {
    public:
        CopyPropagation() = default;

        ~CopyPropagation() override = default;

        bool run(std::vector<base::BasicBlock<InstrType>> &blocks) override {
            bool changed = false;

            for (const auto &block: blocks) {
                find_all_copies(block);
            }

            for (auto &block: blocks) {
                changed |= replace_copies(block);
            }

            return changed;
        }

    private:
        // dest -> src
        std::unordered_map<std::string, std::string> copy_map_;


        void find_all_copies(const base::BasicBlock<InstrType> &block) {
            for (auto &instr: block.instructions()) {
                if (const auto copy = instr.get_if<ir::copy>()) {
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

        bool replace_copies(base::BasicBlock<InstrType> &block) {
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
        void invalidate_destination(const ir::instruction &instr) {
            if (const auto copy = instr.get_if<ir::copy>()) {
                auto destination_value = copy->destination;
                if (destination_value.is_variable() && copy->source.is_constant()) {
                    auto dest = destination_value.get_variable();
                    copy_map_.erase(dest);
                }
            } else if (const auto binary = instr.get_if<ir::binary>()) {
                auto result_value = binary->result;
                if (result_value.is_variable()) {
                    auto dest = result_value.get_variable();
                    copy_map_.erase(dest);
                }
            } else if (const auto unary = instr.get_if<ir::unary>()) {
                auto result_value = unary->result;
                if (result_value.is_variable()) {
                    auto dest = result_value.get_variable();
                    copy_map_.erase(dest);
                }
            }
            // if (const auto copy_instr = instr.get_if<ir::copy>()) {
            //     auto destination_value = copy_instr->destination;
            //     if (destination_value.is_variable() && !copy_instr->source.is_constant()) {
            //         auto dest = destination_value.get_variable();
            //         copy_map_.erase(dest);
            //     }
            // } else if (const auto binary_instr = instr.get_if<ir::binary>()) {
            //     auto result_value = binary_instr->result;
            //     if (result_value.is_variable()) {
            //         auto dest = result_value.get_variable();
            //         copy_map_.erase(dest);
            //     }
            // } else if (const auto unary_instr = instr.get_if<ir::unary>()) {
            //     auto result_value = unary_instr->result;
            //     if (result_value.is_variable()) {
            //         auto dest = result_value.get_variable();
            //         copy_map_.erase(dest);
            //     }
            // }
        }

        std::optional<ir::VirtualReg> replace_operand(ir::VirtualReg &operand) {
            if (operand.is_constant() || !copy_map_.contains(operand.get_variable())) {
                return {};
            }

            return ir::VirtualReg{copy_map_[operand.get_variable()]};
        }

        std::optional<ir::instruction> propagate(const ir::copy &copy) {
            ir::VirtualReg source = copy.source;

            const auto new_source = replace_operand(source);

            if (new_source.has_value()) {
                return ir::instruction{ir::copy{copy.destination, new_source.value()}};
            }

            return {};
        }

        std::optional<ir::instruction> propagate(const ir::binary &binary) {
            ir::VirtualReg left = binary.left;
            ir::VirtualReg right = binary.right;

            const auto new_left = replace_operand(left);
            const auto new_right = replace_operand(right);

            if (new_left.has_value() || new_right.has_value()) {
                return ir::instruction{ir::binary{binary.op, new_left.value(), new_right.value(), binary.result}};
            }

            return {};
        }


        std::optional<ir::instruction> propagate(const ir::unary &unary) {
            ir::VirtualReg operand = unary.value;

            const auto new_operand = replace_operand(operand);

            if (new_operand.has_value()) {
                return ir::instruction(ir::unary(unary.op, new_operand.value(), unary.result));
            }

            return {};
        }

        std::optional<ir::instruction> propagate(const ir::return_ &ret) {
            ir::VirtualReg value = ret.value;

            const auto new_value = replace_operand(value);

            if (new_value.has_value()) {
                return ir::instruction{ir::return_{new_value.value()}};
            }

            return {};
        }

        template<typename T>
        std::optional<ir::instruction> propagate(const T &instr) {
            return {};
        }
    };
}
