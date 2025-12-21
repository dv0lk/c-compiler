#pragma once
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "cfg.hpp"
#include "traits/ir.hpp"
#include "base/base.hpp"

namespace compiler::analysis {

    struct InstrId {
        size_t block_id;
        size_t instr_idx;

        bool operator==(const InstrId&) const = default;
    };

    struct InstrIdHash {
        size_t operator()(const InstrId& id) const {
            return std::hash<size_t>{}(id.block_id) ^ (std::hash<size_t>{}(id.instr_idx) << 16);
        }
    };

    template<typename InstrType>
    class LivenessAnalysis {
    public:
        using traits = InstructionTrait<InstrType>;
        using LiveSet = std::unordered_set<std::string>;

    private:
        //block liveness
        std::unordered_map<size_t, LiveSet> block_live_in_;
        std::unordered_map<size_t, LiveSet> block_live_out_;
        //instruction liveness
        std::unordered_map<InstrId, LiveSet, InstrIdHash> instr_live_in_;
        std::unordered_map<InstrId, LiveSet, InstrIdHash> instr_live_out_;

        const CFG<InstrType>* cfg_ = nullptr;
    public:
        void analyze(const std::vector<BasicBlock<InstrType>>& basic_blocks) {
            CFG<InstrType> cfg;
            cfg.build_nodes(basic_blocks);
            analyze(cfg);
        }

        void analyze(const CFG<InstrType>& cfg) {
            cfg_ = &cfg;
            clear();
            computer_block_liveness(cfg);
            compute_instruction_liveness(cfg);
        }

        [[nodiscard]] const LiveSet& get_instr_live_in(size_t block_id, size_t instr_idx) const {
            static const LiveSet empty;
            auto it = instr_live_in_.find({block_id, instr_idx});
            return it != instr_live_in_.end() ? it->second : empty;
        }

        [[nodiscard]] const LiveSet& get_instr_live_out(size_t block_id, size_t instr_idx) const {
            static const LiveSet empty;
            auto it = instr_live_out_.find({block_id, instr_idx});
            return it != instr_live_out_.end() ? it->second : empty;
        }

        [[nodiscard]] bool is_live_in(size_t block_id, size_t instr_idx, const std::string& var) const {
            const auto& live_in = get_instr_live_in(block_id, instr_idx);
            return live_in.contains(var);
        }

        [[nodiscard]] bool is_live_out(size_t block_id, size_t instr_idx, const std::string& var) const {
            const auto& live_out = get_instr_live_out(block_id, instr_idx);
            return live_out.contains(var);
        }

        [[nodiscard]] bool is_dead(size_t block_id, size_t instr_idx, const InstrType& instr) const {
            auto defs = traits::get_defs(instr);
            if (defs.empty()) return false;

            const auto& live_out = get_instr_live_out(block_id, instr_idx);
            for (const auto& def : defs) {
                if (live_out.contains(def)) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] const LiveSet& get_block_live_in(size_t block_id) const {
            static const LiveSet empty;
            auto it = block_live_in_.find(block_id);
            return it != block_live_in_.end() ? it->second : empty;
        }

        [[nodiscard]] const LiveSet& get_block_live_out(size_t block_id) const {
            static const LiveSet empty;
            auto it = block_live_out_.find(block_id);
            return it != block_live_out_.end() ? it->second : empty;
        }

        [[nodiscard]] LiveSet get_all_live_variables() const {
            LiveSet all;
            for (const auto& [id, live_set] : block_live_in_) {
                all.insert(live_set.begin(), live_set.end());
            }
            for (const auto& [id, live_set] : block_live_out_) {
                all.insert(live_set.begin(), live_set.end());
            }
            return all;
        }

        void clear() {
            block_live_in_.clear();
            block_live_out_.clear();
            instr_live_in_.clear();
            instr_live_out_.clear();

            for (size_t id : cfg_->get_block_ids()) {
                block_live_in_[id] = LiveSet{};
                block_live_out_[id] = LiveSet{};
            }
        }

    private:
        void computer_block_liveness(const CFG<InstrType>& cfg) {
            bool changed = true;
            while (changed) {
                changed = false;

                for (size_t id : cfg.get_block_ids() | std::views::reverse) {
                    if (id == START_NODE || id == EXIT_NODE) continue;

                    auto* node = cfg.find_node(id);
                    if (!node || node->empty()) continue;

                    LiveSet new_live_out;
                    for (const auto succ_id : node->successors) {
                        const auto& succ_live_in = block_live_in_[succ_id];
                        new_live_out.insert(succ_live_in.begin(), succ_live_in.end());
                    }

                    LiveSet new_live_in = new_live_out;
                    for (const auto& instr : node->block->instructions() | std::views::reverse) {
                        for (const auto& def : traits::get_defs(instr)) {
                            new_live_in.erase(def);
                        }

                        for (const auto& use : traits::get_uses(instr)) {
                            new_live_in.insert(use);
                        }
                    }

                    if (new_live_in != block_live_in_[id] || new_live_out != block_live_out_[id]) {
                        changed = true;
                        block_live_in_[id] = std::move(new_live_in);
                        block_live_out_[id] = std::move(new_live_out);
                    }
                }
            }
        }

        void compute_instruction_liveness(const CFG<InstrType>& cfg) {
            for (size_t block_id : cfg.get_block_ids()) {
                if (block_id == START_NODE || block_id == EXIT_NODE) continue;

                auto* node = cfg.find_node(block_id);
                if (!node || node->empty()) continue;

                const auto& instructions = node->block->instructions();
                size_t num_instrs = instructions.size();

                LiveSet current_live = block_live_out_[block_id];

                for (size_t i = num_instrs; i > 0; --i) {
                    size_t idx = i - 1;
                    const auto& instr = instructions[idx];
                    InstrId instr_id{block_id, idx};

                    instr_live_out_[instr_id] = current_live;

                    for (const auto& def : traits::get_defs(instr)) {
                        current_live.erase(def);
                    }
                    for (const auto& use : traits::get_uses(instr)) {
                        current_live.insert(use);
                    }

                    instr_live_in_[instr_id] = current_live;
                }
            }
        }
    };
}
