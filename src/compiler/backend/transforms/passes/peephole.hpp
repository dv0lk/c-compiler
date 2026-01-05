#pragma once
#include "transforms/transform.hpp"
#include "x86/types/instruction.hpp"

namespace compiler::transforms {

    class Peephole : public Transform<x86::Instruction> {
    public:
        Peephole() = default;

        ~Peephole() override = default;

        bool run(std::vector<x86::Instruction>& instructions) override {
            if (instructions.empty())
                return false;

            std::vector<x86::Instruction> result;
            result.push_back(std::move(instructions[0]));

            for (size_t i = 1; i < instructions.size(); i++) {
                auto* mov = instructions[i].get_if<x86::Mov>();

                if (!mov) {
                    result.push_back(std::move(instructions[i]));
                    continue;
                }

                if (is_self_move(mov)) {
                    continue;
                }

                auto* prev_mov = result.back().get_if<x86::Mov>();
                if (prev_mov && is_redundant_pair(mov, prev_mov)) {
                    continue;
                }

                result.push_back(std::move(instructions[i]));
            }

            bool changed = result.size() != instructions.size();
            instructions = std::move(result);
            return changed;
        }

    private:
        static bool is_self_move(const x86::Mov* mov) {
            return mov->destination == mov->source;
        }

        static bool is_redundant_pair(const x86::Mov* first, const x86::Mov* second) {
            return first->source == second->destination && first->destination == second->source;
        }
    };

} // namespace compiler::transforms