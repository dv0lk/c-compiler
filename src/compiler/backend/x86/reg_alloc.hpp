#pragma once
#include <algorithm>
#include <array>
#include <optional>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <vector>

#include "analysis/cfg.hpp"
#include "analysis/liveness.hpp"
#include "analysis/traits/x86.hpp"
#include "structure/function.hpp"
#include "types/types.hpp"


namespace compiler::x86 {
    class InterferenceGraph {
    public:
        using traits = InstructionTrait<Instruction>;

        static InterferenceGraph get_graph(const CFG<Instruction> &cfg,
                                           const LivenessAnalysis<Instruction> &liveness) {
            InterferenceGraph graph;

            for (size_t block_id : cfg.get_block_ids()) {
                if (block_id == START_NODE || block_id == EXIT_NODE) continue;

                auto* node = cfg.find_node(block_id);
                if (!node || node->empty()) continue;

                size_t instr_index = 0;
                for (const auto &instr: node->block->instructions()) {
                    //get all live variables at this instruction
                    auto live_out = liveness.get_instr_live_out(block_id, instr_index);
                    //when variable is defined, connect it to all nodes that are currently live
                    for (const auto &def: traits::get_defs(instr)) {
                        graph.add_node(def);
                        for (const auto &live: live_out) {
                            graph.add_edge(def, live);
                        }
                    }

                    //ensures that we add function params to the graph todo, this might be useless, double check
                    for (const auto &use: traits::get_uses(instr)) {
                        graph.add_node(use);
                    }

                    ++instr_index;
                }
            }
            return graph;
        }

        void add_node(const std::string &name) {
            if (!nodes_.contains(name)) {
                nodes_.insert(name);
                edges_[name] = {};
            }
        }

        void add_edge(const std::string &one, const std::string &two) {
            if (one == two) return;
            add_node(one);
            add_node(two);
            edges_[one].insert(two);
            edges_[two].insert(one);
        }

        [[nodiscard]] size_t degree(const std::string &node) const {
            return edges_.at(node).size();
        }

        [[nodiscard]] const std::unordered_set<std::string> &neighbors(const std::string &node) {
            return edges_[node];
        }

        [[nodiscard]] const std::unordered_set<std::string> &nodes() const {
            return nodes_;
        }

        void remove_node(const std::string &node) {
            for (const auto &edge: edges_[node]) {
                edges_[edge].erase(node);
            }
            edges_.erase(node);
            nodes_.erase(node);
        }

        [[nodiscard]] bool empty() const {
            return nodes_.empty();
        }

        [[nodiscard]] auto next() const {
            return *nodes_.begin();
        }

        [[nodiscard]] std::string max_degree_node() const {
            return *std::ranges::max_element(nodes_, std::less{}, [this](const auto &node) {
                return degree(node);
            });
        }

    private:
        std::unordered_set<std::string> nodes_;
        std::unordered_map<std::string, std::unordered_set<std::string> > edges_;
    };

    class RegisterAllocator {
    public:
        static constexpr std::array ALLOCATABLE_REGS = {
            RegType::RBX, RegType::RCX, RegType::RDX, RegType::RSI, RegType::RDI, RegType::R8, RegType::R9, RegType::R10, RegType::R11, RegType::R12, RegType::R13, RegType::R14, RegType::R15
        };

        static constexpr size_t K = ALLOCATABLE_REGS.size();

        RegisterAllocator() = default;

        void run_on_program(Program<Instruction> &program) {
            for (auto &func: program.functions()) {
                run_on_function(func);
            }
        }

        void run_on_function(Function<Instruction> &function) {
            reset();

            auto cfg = CFG<Instruction>::from_function(function);
            auto liveness = LivenessAnalysis<Instruction>::get_analysis(cfg);
            auto graph = InterferenceGraph::get_graph(cfg, liveness);

            color_graph(graph);

            rewrite_function(function);

            add_prologue(function);
            add_epilogue(function);
        }

    private:
        void reset() {
            allocation_.clear();
            used_callee_saved_.clear();
            curr_stack_offset_ = -8;
        }

        //chaitin briggs graph coloring algorithm
        void color_graph(InterferenceGraph &graph) {
            //make a copy of graph for later
            auto saved_graph = graph;

            std::stack<std::string> stack;

            //simplify phase
            while (!graph.empty()) {
                auto node = graph.next();

                //we push node with the highest degree to the stack early
                //There is a chance colors will become available when we get to it
                if (graph.degree(node) >= K) {
                    node = graph.max_degree_node();
                }

                stack.emplace(node);
                graph.remove_node(node);
            }


            //select phase. Pop stack, try to assign the color, if we can't, spill
            while (!stack.empty()) {
                auto node = stack.top();
                stack.pop();

                //gather all colors used by neighbors
                std::unordered_set<RegType> used_regs;
                for (const auto &neighbor: saved_graph.neighbors(node)) {
                    if (allocation_.contains(neighbor)) {
                        used_regs.insert(allocation_[neighbor]);
                    }
                }

                //try to assign unused register
                std::optional<RegType> reg;
                for (auto r: ALLOCATABLE_REGS) {
                    if (!used_regs.contains(r)) {
                        reg = r;
                        break;
                    }
                }

                if (reg.has_value()) {
                    allocation_[node] = reg.value();
                    used_callee_saved_.insert(reg.value());
                } else {
                    spilled_[node] = curr_stack_offset_;
                    curr_stack_offset_ -= 8;
                }
            }
        }

        Operand get_new_operand(const Operand &operand) {
            if (!operand.holds<PseudoRegister>()) {
                return operand;
            }

            const auto pseudo_reg = operand.get_if<PseudoRegister>();
            if (allocation_.contains(pseudo_reg->name)) {
                return Register(allocation_[pseudo_reg->name]);
            }

            if (spilled_.contains(pseudo_reg->name)) {
                return Mem(spilled_[pseudo_reg->name]);
            }

            throw std::runtime_error("Pseudo register not alloacted");
        }

        void rewrite_instruction(Instruction &instr) {
            if (auto *mov = instr.get_if<Mov>()) {
                mov->destination = get_new_operand(mov->destination);
                mov->source = get_new_operand(mov->source);
            } else if (auto *add = instr.get_if<Add>()) {
                add->destination = get_new_operand(add->destination);
                add->source = get_new_operand(add->source);
            } else if (auto *sub = instr.get_if<Sub>()) {
                sub->destination = get_new_operand(sub->destination);
                sub->source = get_new_operand(sub->source);
            } else if (auto *imul = instr.get_if<Imul>()) {
                imul->destination = get_new_operand(imul->destination);
                imul->source = get_new_operand(imul->source);
            } else if (auto *idiv = instr.get_if<Idiv>()) {
                idiv->value = get_new_operand(idiv->value);
            } else if (auto *neg = instr.get_if<Neg>()) {
                neg->value = get_new_operand(neg->value);
            } else if (auto *not_ = instr.get_if<Not>()) {
                not_->value = get_new_operand(not_->value);
            } else if (auto *cmp = instr.get_if<Cmp>()) {
                cmp->destination = get_new_operand(cmp->destination);
                cmp->source = get_new_operand(cmp->source);
            } else if (auto *setcc = instr.get_if<SetCC>()) {
                setcc->value = get_new_operand(setcc->value);
            } else if (auto *push = instr.get_if<Push>()) {
                push->value = get_new_operand(push->value);
            } else if (auto *pop = instr.get_if<Pop>()) {
                pop->value = get_new_operand(pop->value);
            }
        }

        void rewrite_function(Function<Instruction> &function) {
            for (auto &instr: function.instructions()) {
                rewrite_instruction(instr);
            }
        }

        void add_prologue(Function<Instruction>& function) {
            if (function.empty())
                return;

            std::vector<Instruction> new_instructions;
            new_instructions.emplace_back(Push(Register(RegType::RBP)));
            new_instructions.emplace_back(Mov(Register(RegType::RBP), Register(RegType::RSP)));

            //push any registers we use to stack so we can restore them
            for (const auto reg : used_callee_saved_) {
                new_instructions.emplace_back(Push(Register(reg)));
            }

            //allocate stack space for any spilled variables
            if (curr_stack_offset_ + 8 < 0) {
                new_instructions.emplace_back(Sub(Register(RegType::RSP), Imm(-curr_stack_offset_ - 8)));
            }

            auto& old_instr = function.instructions();
            for (auto& instr : old_instr) {
                new_instructions.emplace_back(std::move(instr));
            }

            old_instr = std::move(new_instructions);
        }

        void add_epilogue(Function<Instruction>& function) {
            auto& old_instr = function.instructions();
            std::vector<Instruction> new_instructions;

            for (auto& instr : old_instr) {
                if (instr.holds<Ret>()) {
                    if (curr_stack_offset_ + 8 < 0) {
                        new_instructions.emplace_back(Add(Register(RegType::RSP), Imm(-curr_stack_offset_ - 8)));
                    }

                    //pop from the stack in reverse
                    for (auto reg : used_callee_saved_ | std::views::reverse) {
                        new_instructions.emplace_back(Pop(Register(reg)));
                    }

                    new_instructions.emplace_back(Pop(Register(RegType::RBP)));
                }
                new_instructions.push_back(std::move(instr));
            }

            old_instr = std::move(new_instructions);
        }



        std::unordered_map<std::string, RegType> allocation_;
        std::unordered_map<std::string, int> spilled_;
        std::set<RegType> used_callee_saved_;
        int curr_stack_offset_ = -8;
    };
}
