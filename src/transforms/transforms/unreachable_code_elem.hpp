#pragma once
#include <vector>

#include "ir/types/basic_block_t.hpp"
#include "transforms/transform.hpp"
#include "analysis/cfg.hpp"

namespace compiler::transforms {
    class UnreachableCodeElem : public Transform<std::vector<ir::basic_block_t>> {
    public:
        UnreachableCodeElem() = default;
        ~UnreachableCodeElem() override = default;

        bool run(std::vector<ir::basic_block_t>& blocks) override {
            auto a = cfg::cfg::get_cfg(blocks);
            return false;
        }

    private:
        void unrecahble_blocks(cfg::cfg& tmp, std::vector<ir::basic_block_t>& blocks) {
            std::vector<ir::instruction> unreachable_instructions;

            for (const auto& node : tmp.get_nodes()) {
                if (node.predecessors.empty()) {
                    for (const auto& instr : node.block->instructions()) {

                    }
                }
            }

        }
    };
}
