#pragma once
#include "flow_graph/flow_graph.hpp"

namespace compiler {
    class CopyPropagation {
    public:
        using FlowGraphType = FlowGraph<ir::ir_instruction>;
        using NodeType = Node<ir::ir_instruction>;

        bool apply(FlowGraphType& graph);

    private:
        // TODO rethink this shit, I don't like it
        using InstructionKey = std::pair<int, size_t>; // <block_id, instruction_index>
        struct PairHash {
            std::size_t operator()(const InstructionKey& p) const {
                return std::hash<int>()(p.first) ^ (std::hash<size_t>()(p.second) << 1);
            }
        };

        std::unordered_map<InstructionKey, std::vector<ir::ir_copy>, PairHash> annotated_instructions;
        std::unordered_map<int, std::vector<ir::ir_copy> > annotated_blocks;

        void find_reaching_copies(FlowGraphType& graph);

        static std::vector<ir::ir_copy> find_all_copy_instr(const FlowGraphType& graph);

        std::vector<ir::ir_copy> meet(const NodeType& node, const std::vector<ir::ir_copy>& all_copies);

        static bool contains_instruction(const std::vector<ir::ir_copy>& copy_instructions,
                                         const ir::ir_copy& instructions);

        void annotate_instruction(int block_id, size_t instr_index, const std::vector<ir::ir_copy>& reaching_copies);

        std::vector<ir::ir_copy> get_instruction_annotations(int block_id, size_t instr_index);

        void annotate_block(const NodeType& node, const std::vector<ir::ir_copy>& reaching_copies);

        std::vector<ir::ir_copy> get_block_annotation(int node_id);

        void kill_copies(std::vector<ir::ir_copy>& reaching_copies, const ir::ir_value& value);

        void transfer(NodeType& node, std::vector<ir::ir_copy>& initial_reaching_copies);

        ir::ir_value replace_operand(ir::ir_value& operand, std::vector<ir::ir_copy>& reaching_copies);

        std::optional<ir::ir_instruction> rewrite_instruction(ir::ir_copy& copy, std::vector<ir::ir_copy>& reaching_copies);

        std::optional<ir::ir_instruction> rewrite_instruction(ir::ir_unary& unary, std::vector<ir::ir_copy>& reaching_copies);

        std::optional<ir::ir_instruction> rewrite_instruction(ir::ir_binary& binary, std::vector<ir::ir_copy>& reaching_copies);

        std::optional<ir::ir_instruction> rewrite_instruction(ir::ir_call& binary, std::vector<ir::ir_copy>& reaching_copies);

        //default case for every instruction we don't handle
        template <typename T, typename T2>
        std::optional<ir::ir_instruction> rewrite_instruction(T& t, T2&) {
            return t;
        }
    };
}
