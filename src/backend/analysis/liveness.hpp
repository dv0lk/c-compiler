#pragma once
#include <algorithm>
#include <set>

#include "cfg.hpp"
#include "traits/ir.hpp"
#include "base/base.hpp"
namespace compiler::analysis {
    template<typename InstrType>
    class LivenessAnalysis {
    public:
        using traits = InstructionTrait<InstrType>;

        void analyze(const std::vector<BasicBlock<InstrType>>& basic_blocks) {
            CFG<InstrType> cfg;
            cfg.build_nodes(basic_blocks);
            analyze(cfg);
        }

        void analyze(const CFG<InstrType>& cfg) {
            auto ids = cfg.get_block_ids();
            std::ranges::sort(ids);

            bool changed = true;
            while (changed) {
                changed = false;

                for (size_t id : ids | std::views::reverse) {
                    if (id == CFG<InstrType>::START_NODE || id == CFG<InstrType>::EXIT_NODE) continue;
                    auto node = cfg.find_node(id);
                    if (!node || node->empty()) continue;

                    LiveSet new_live_out;
                    for (const auto succ_id : node->successors) {
                        const auto& succ_live_in = block_live_in_[succ_id];
                        new_live_out.insert(succ_live_in.begin(), succ_live_in.end());
                    }

                    LiveSet new_live_in = new_live_out;
                    // if ()

                }
            }
        }

    private:
        using LiveSet = std::unordered_set<std::string>;
        std::unordered_map<size_t, LiveSet> block_live_in_;
        std::unordered_map<size_t, LiveSet> block_live_out_;
        std::unordered_map<size_t, LiveSet> instr_live_in_;
        std::unordered_map<size_t, LiveSet> instr_live_out_;


        void remove_defined_registers() {

        }

        void add_used_registers() {

        }
    };
}