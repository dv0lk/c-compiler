#pragma once
#include <memory>
#include <vector>
#include "transform.hpp"
#include "structure/program.hpp"

namespace compiler {
    template<typename InstrType>
    class TransformManager {
    public:
        TransformManager() = default;

        ~TransformManager() = default;

        template<typename PassType>
        void register_transform() {
            auto instance = std::make_unique<PassType>();
            passes_.emplace_back(std::move(instance));
        }

        bool run_on_function(Function<InstrType> &function) {
            bool changed = false;
            for (auto &pass: passes_) {
                changed |= pass->run(function.basic_blocks());
            }
            return changed;
        }

        bool run_on_program(Program<InstrType> &program) {
            bool changed = false;
            for (auto &function: program) {
                changed |= run_on_function(function);
            }
            return changed;
        }

    private:
        using bb_t = std::vector<BasicBlock<InstrType> >;
        using transform_ptr = std::shared_ptr<Transform<bb_t> >;
        std::vector<transform_ptr> passes_;
    };
}
