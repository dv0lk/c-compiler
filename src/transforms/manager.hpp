#pragma once
#include <memory>
#include <vector>
#include "transform.hpp"

namespace compiler {
    template<typename InstrType>
    class TransformManager {
    private:
        using TransformPtr = std::shared_ptr<Transform<InstrType>>;
        std::vector<TransformPtr> passes_;
    public:
        TransformManager() = default;

        ~TransformManager() = default;

        template <typename PassType>
        void register_transform() {
            auto instance = std::make_unique<PassType>();
            passes_.emplace_back(std::move(instance));
        }

        bool run_on_function(ir::function_t& function) {
            bool changed = false;
            for (auto& pass : passes_) {
                changed |= pass->run(function.blocks);
            }
            return changed;
        }

        bool run_on_program(ir::program_t& program) {
            bool changed = false;
            for (auto& function : program) {
                changed |= run_on_function(function);
            }
            return changed;
        }

    };
}
