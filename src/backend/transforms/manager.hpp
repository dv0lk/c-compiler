#pragma once
#include <memory>
#include <vector>
#include "transform.hpp"
#include "base/program.hpp"
namespace compiler {
    template<typename InstrType>
    class TransformManager {
    private:
        using bb_t = std::vector<base::BasicBlock<InstrType>>;
        using transform_ptr = std::shared_ptr<Transform<bb_t>>;
        std::vector<transform_ptr> passes_;
    public:
        TransformManager() = default;

        ~TransformManager() = default;

        template <typename PassType>
        void register_transform() {
            auto instance = std::make_unique<PassType>();
            passes_.emplace_back(std::move(instance));
        }

        bool run_on_function(base::Function<InstrType>& function) {
            bool changed = false;
            for (auto& pass : passes_) {
                changed |= pass->run(function.basic_blocks());
            }
            return changed;
        }

        bool run_on_program(base::Program<InstrType>& program) {
            bool changed = false;
            for (auto& function : program) {
                changed |= run_on_function(function);
            }
            return changed;
        }

    };
}
