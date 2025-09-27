//TODO finsih registry allocation

// #pragma once
// #include <algorithm>
// #include <array>
// #include <deque>
// #include <ranges>
// #include <set>
//
// #include "flow_graph/flow_graph.hpp"
//
// namespace compiler {
//     struct OperandInfo {
//         std::vector<x86::Operand> used;
//         std::vector<x86::Operand> updated;
//     };
//
//     struct InterferenceNode {
//         x86::Operand id;
//         double spill_cost = 0.0;
//         std::vector<x86::Operand> neighbors;
//         std::optional<int> color = {};
//         bool pruned = false;
//
//         explicit InterferenceNode(const x86::Operand& id)
//             : id(id) {}
//
//         void add_neighbor(const x86::Operand& operand) {
//             neighbors.emplace_back(operand);
//         }
//     };
//
//     class Register {
//     private:
//         const std::array<x86::Operand, 12> base_registers = {
//             x86::registers::RAX, x86::registers::RBX, x86::registers::RCX,
//             x86::registers::RDX, x86::registers::RDI, x86::registers::RSI,
//             x86::registers::R8, x86::registers::R9, x86::registers::R12,
//             x86::registers::R13, x86::registers::R14, x86::registers::R15
//         };
//
//         //TODO I really need to rethink this
//         using InstructionKey = std::pair<int, size_t>; // (block_id, instruction_index)
//         struct PairHash {
//             std::size_t operator()(const InstructionKey& p) const {
//                 return std::hash<int>()(p.first) ^ (std::hash<size_t>()(p.second) << 1);
//             }
//         };
//
//         std::unordered_map<InstructionKey, std::vector<x86::Register>, PairHash> annotated_instructions;
//         std::unordered_map<int, std::vector<x86::Register> > annotated_blocks;
//
//         using NodeType = Node<x86::instruction>;
//         std::vector<InterferenceNode> interference_graph;
//         FlowGraph<x86::instruction> cfg;
//
//
//         void build_graph(const std::vector<x86::instruction>& instructions);
//
//         void setup_base_graph();
//
//         void add_pseudoregisters(const std::vector<x86::instruction>& instructions);
//
//         void analyze_liveness();
//
//         void annotate_block(const NodeType& node, const std::vector<x86::Register>& registers);
//
//         std::vector<x86::Register> get_block_annotation(int id);
//
//         std::vector<x86::Register> meet(const NodeType& node);
//
//         static OperandInfo find_used_and_update(const x86::instruction& instruction);
//
//         void transfer(NodeType& node, const std::vector<x86::Register>& end_live_registers);
//
//         void annotate_instruction(int block_id, size_t instr_index, const std::vector<x86::Register>& registers);
//
//         std::vector<x86::Register> get_instruction_annotation(int block_id, size_t instr_index);
//
//         void add_edges();
//
//         void add_interference_edge(const x86::Register& reg1, const x86::Register& reg2);
//     };
//
//     inline void Register::build_graph(const std::vector<x86::instruction>& instructions) {
//         setup_base_graph();
//         add_pseudoregisters(instructions);
//         cfg.generate_flowgraph(instructions);
//     }
//
//     inline void Register::setup_base_graph() {
//         for (const auto& reg : base_registers) {
//             interference_graph.emplace_back(reg);
//         }
//
//         for (auto& node : interference_graph) {
//             for (const auto& reg : base_registers) {
//                 if (node.id != reg)
//                     node.add_neighbor(reg);
//             }
//         }
//     }
//
//     inline void Register::add_pseudoregisters(const std::vector<x86::instruction>& instructions) {
//         auto add_if_pseudoregister = [&](const x86::Operand& op) {
//             if (std::holds_alternative<x86::PseudoRegister>(op)) {
//                 interference_graph.emplace_back(op);
//             }
//         };
//
//         for (const auto& instruction : instructions) {
//             if (const auto mov = std::get_if<x86::mov>(&instruction)) {
//                 add_if_pseudoregister(mov->source);
//                 add_if_pseudoregister(mov->destination);
//                 continue;
//             }
//
//             if (const auto neg = std::get_if<x86::neg>(&instruction)) {
//                 add_if_pseudoregister(neg->value);
//                 continue;
//             }
//
//             if (const auto not_ = std::get_if<x86::not_>(&instruction)) {
//                 add_if_pseudoregister(not_->value);
//                 continue;
//             }
//
//             if (const auto add = std::get_if<x86::add>(&instruction)) {
//                 add_if_pseudoregister(add->source);
//                 add_if_pseudoregister(add->destination);
//                 continue;
//             }
//
//             if (const auto sub = std::get_if<x86::sub>(&instruction)) {
//                 add_if_pseudoregister(sub->source);
//                 add_if_pseudoregister(sub->destination);
//                 continue;
//             }
//
//             if (const auto imul = std::get_if<x86::imul>(&instruction)) {
//                 add_if_pseudoregister(imul->source);
//                 add_if_pseudoregister(imul->destination);
//                 continue;
//             }
//
//             if (const auto idiv = std::get_if<x86::idiv>(&instruction)) {
//                 add_if_pseudoregister(idiv->value);
//                 continue;
//             }
//
//             if (const auto cmp = std::get_if<x86::cmp>(&instruction)) {
//                 add_if_pseudoregister(cmp->source);
//                 add_if_pseudoregister(cmp->destination);
//                 continue;
//             }
//
//             if (const auto set_cc = std::get_if<x86::set_cc>(&instruction)) {
//                 add_if_pseudoregister(set_cc->value);
//                 continue;
//             }
//
//             if (const auto push = std::get_if<x86::push>(&instruction)) {
//                 add_if_pseudoregister(push->value);
//                 continue;
//             }
//
//             if (const auto pop = std::get_if<x86::pop>(&instruction)) {
//                 add_if_pseudoregister(pop->value);
//                 continue;
//             }
//         }
//     }
//
//     inline void Register::analyze_liveness() {
//         std::deque<int> worklist;
//
//         for (const auto& node : cfg.nodes) {
//             if (node.id == ENTRY || node.id == EXIT)
//                 continue;
//
//             worklist.push_back(node.id);
//             annotate_block(node, {});
//         }
//
//         while (!worklist.empty()) {
//             const auto block_id = worklist.front();
//             worklist.pop_front();
//
//             auto node_it = std::ranges::find_if(cfg.nodes,
//                                                 [block_id](const auto& n) {
//                                                     return n.id == block_id;
//                                                 });
//
//             if (node_it == cfg.nodes.end())
//                 continue;
//
//         }
//     }
//
//     inline void Register::annotate_block(const NodeType& node, const std::vector<x86::Register>& registers) {
//         annotated_blocks[node.id] = registers;
//     }
//
//     inline void Register::annotate_instruction(int block_id, size_t instr_index, const std::vector<x86::Register>& registers) {
//         annotated_instructions[{block_id, instr_index}] = registers;
//     }
//
//     inline std::vector<x86::Register> Register::get_instruction_annotation(int block_id, size_t instr_index) {
//         const auto it = annotated_instructions.find({block_id, instr_index});
//         if (it != annotated_instructions.end())
//             return it->second;
//
//         throw std::runtime_error("get_instruction_annotation on unknown instruction");
//     }
//
//     inline void Register::add_interference_edge(const x86::Register& reg1, const x86::Register& reg2) {
//         if (reg1 == reg2)
//             return;
//
//         auto find_node = [this](const x86::Register& reg) {
//             return std::ranges::find_if(interference_graph,
//                                         [&reg](const auto& n) {
//                                             return n.id == x86::Operand(reg);
//                                         });
//         };
//
//         const auto node1 = find_node(reg1);
//         const auto node2 = find_node(reg2);
//
//         if (node1 != interference_graph.end() && node2 != interference_graph.end()) {
//             node1->add_neighbor(x86::Operand(reg2));
//             node2->add_neighbor(x86::Operand(reg1));
//         }
//
//     }
//
//     inline void Register::add_edges() {
//         for (const auto& node : cfg.nodes) {
//             if (node.id == ENTRY || node.id == EXIT)
//                 continue;
//
//             for (const auto& [index, instruction] : std::views::enumerate(node.instructions)) {
//                 auto const [used, updated] = find_used_and_update(instruction);
//                 auto live_registers = get_instruction_annotation(node.id, index);
//
//                 if (const auto& mov = std::get_if<x86::mov>(&instruction)) {
//                     if (std::holds_alternative<x86::Register>(mov->destination)) {
//                         const auto destination_reg = std::get<x86::Register>(mov->destination);
//                         std::erase(live_registers, destination_reg);
//                     }
//                 }
//
//                 for (const auto& live_reg : live_registers) {
//                     for (const auto& updated_op : updated) {
//                         if (std::holds_alternative<x86::Register>(updated_op)) {
//                             add_interference_edge(live_reg, std::get<x86::Register>(updated_op));
//                         }
//                     }
//                 }
//             }
//         }
//     }
//
//
//     inline std::vector<x86::Register> Register::get_block_annotation(const int id) {
//         const auto it = annotated_blocks.find(id);
//         if (it != annotated_blocks.end())
//             return it->second;
//
//         throw std::runtime_error("No annotation for block");
//     }
//
//     inline std::vector<x86::Register> Register::meet(const NodeType& node) {
//         std::vector<x86::Register> live_registers;
//
//         for (const auto successor_id : node.successors) {
//             if (successor_id == EXIT) {
//                 live_registers.emplace_back(x86::registers::RAX);
//                 continue;
//             }
//
//             if (successor_id == ENTRY) {
//                 throw std::runtime_error("Malformed control flow graph");
//             }
//
//             for (const auto& reg : get_block_annotation(successor_id)) {
//                 live_registers.emplace_back(reg);
//             }
//         }
//
//         return live_registers;
//     }
//
//     inline OperandInfo Register::find_used_and_update(const x86::instruction& instruction) {
//         if (const auto mov = std::get_if<x86::mov>(&instruction))
//             return OperandInfo{.used = {mov->source}, .updated = {mov->destination}};
//
//         if (const auto neg = std::get_if<x86::neg>(&instruction))
//             return OperandInfo{.used = {neg->value}, .updated = {neg->value}};
//
//         if (const auto not_ = std::get_if<x86::not_>(&instruction))
//             return OperandInfo{.used = {not_->value}, .updated = {not_->value}};
//
//         if (const auto add = std::get_if<x86::add>(&instruction))
//             return OperandInfo{.used = {add->source, add->destination}, .updated = {add->destination}};
//
//         if (const auto sub = std::get_if<x86::sub>(&instruction))
//             return OperandInfo{.used = {sub->source, sub->destination}, .updated = {sub->destination}};
//
//         if (const auto imul = std::get_if<x86::imul>(&instruction))
//             return OperandInfo{.used = {imul->source, imul->destination}, .updated = {imul->destination}};
//
//         if (const auto cmp = std::get_if<x86::cmp>(&instruction))
//             return OperandInfo{.used = {cmp->source, cmp->destination}, .updated = {}};
//
//         if (const auto set_cc = std::get_if<x86::set_cc>(&instruction))
//             return OperandInfo{.used = {}, .updated = {set_cc->value}};
//
//         if (const auto push = std::get_if<x86::push>(&instruction))
//             return OperandInfo{.used = {push->value}, .updated = {}};
//
//         if (const auto pop = std::get_if<x86::pop>(&instruction))
//             return OperandInfo{.used = {}, .updated = {pop->value}};
//
//         if (const auto idiv = std::get_if<x86::idiv>(&instruction))
//             return OperandInfo{
//                 .used = {idiv->value, x86::registers::RAX, x86::registers::RDX},
//                 .updated = {x86::registers::RAX, x86::registers::RDX}
//             };
//
//         if (const auto cdq = std::get_if<x86::cdq>(&instruction))
//             return OperandInfo{.used = {x86::registers::RAX}, .updated = {x86::registers::RDX}};
//
//         //TODO implement CALL!
//
//         return OperandInfo{.used = {}, .updated = {}};
//     }
//
//     inline void Register::transfer(NodeType& node, const std::vector<x86::Register>& end_live_registers) {
//         auto current_live_registers = end_live_registers;
//
//         for (const auto& [index, instruction] : std::views::enumerate(std::views::reverse(node.instructions))) {
//             const auto actual_index = node.instructions.size() - 1 - index;
//             annotate_instruction(node.id, actual_index, current_live_registers);
//
//             auto [used, updated] = find_used_and_update(instruction);
//
//             for (const auto& reg : updated) {
//                 if (std::holds_alternative<x86::Register>(reg))
//                     std::erase(current_live_registers, std::get<x86::Register>(reg));
//             }
//
//             for (const auto& reg : used) {
//                 if (std::holds_alternative<x86::Register>(reg))
//                     std::erase(current_live_registers, std::get<x86::Register>(reg));
//             }
//         }
//
//         annotate_block(node, current_live_registers);
//     }
// }
