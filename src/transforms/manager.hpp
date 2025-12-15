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

        bool run_all(InstrType& data) {
            bool changed = false;
            for ( auto& pass : passes_) {
                changed |= pass->run(data);
            }
            return changed;
        }

    };
}
