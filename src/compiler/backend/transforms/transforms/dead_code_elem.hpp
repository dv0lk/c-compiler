#pragma once
#include <algorithm>
#include <unordered_set>
#include "analysis/cfg.hpp"
#include "analysis/liveness.hpp"
#include "analysis/traits/traits.hpp"
#include "transforms/transform.hpp"

namespace compiler::transforms {
    template<typename InstrType>
    class DeadCodeElim : public Transform<InstrType> {
    public:
        using traits = InstructionTrait<InstrType>;

        DeadCodeElim() = default;

        ~DeadCodeElim() override = default;

        bool run(std::vector<InstrType> &instructions) override {
            if (instructions.empty()) return false;

            bool any_changed = false;
            bool changed = true;

            while (changed) {
                changed = false;

                auto cfg = CFG<InstrType>::from_instructions(instructions);
                auto liveness = LivenessAnalysis<InstrType>::get_analysis(cfg);

                std::vector<size_t> block_ids;
                for (size_t id: cfg.get_block_ids()) {
                    if (id != START_NODE && id != EXIT_NODE) {
                        block_ids.push_back(id);
                    }
                }
                std::ranges::sort(block_ids);

                std::unordered_set<size_t> dead_indices;
                size_t global_index = 0;
                for (size_t block_id: block_ids) {
                    auto *node = cfg.find_node(block_id);
                    if (!node || node->empty()) continue;

                    size_t instr_index = 0;
                    for (const auto &instr: node->block->instructions()) {
                        if (is_dead_instruction(instr, liveness, block_id, instr_index)) {
                            dead_indices.insert(global_index);
                            changed = true;
                        }
                        ++instr_index;
                        ++global_index;
                    }
                }

                if (!dead_indices.empty()) {
                    std::vector<InstrType> new_instructions;
                    new_instructions.reserve(instructions.size() - dead_indices.size());

                    for (size_t i = 0; i < instructions.size(); ++i) {
                        if (!dead_indices.contains(i)) {
                            new_instructions.push_back(std::move(instructions[i]));
                        }
                    }

                    instructions = std::move(new_instructions);
                    any_changed = true;
                }
            }

            return any_changed;
        }

    private:
        static bool is_dead_instruction(const InstrType &instr,
                                        const LivenessAnalysis<InstrType> &liveness,
                                        size_t block_id,
                                        size_t instr_index) {
            if (traits::is_terminator(instr) || traits::is_label(instr) || traits::has_side_effects(instr)) {
                return false;
            }

            auto defs = traits::get_defs(instr);
            if (defs.empty()) {
                return false;
            }

            const auto &live_out = liveness.get_instr_live_out(block_id, instr_index);
            for (const auto &def: defs) {
                if (live_out.contains(def)) {
                    return false;
                }
            }

            return true;
        }
    };
}
