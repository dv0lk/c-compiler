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
            local_passes_.emplace_back(std::make_unique<PassType>());
        }

        bool run_on_function(Function<InstrType> &function) {
            bool changed = false;
            for (auto &pass: local_passes_) {
                changed |= pass->run(function.instructions());
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
        using local_transform_ptr = std::unique_ptr<Transform<InstrType>>;
        std::vector<local_transform_ptr> local_passes_;
    };
}
