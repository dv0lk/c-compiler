#pragma once

#include <vector>

namespace compiler {
    template <typename InstrType>
    class Transform {
    public:
        Transform() = default;
        virtual ~Transform() = default;

        [[nodiscard]] virtual bool run(std::vector<InstrType>& instructions) = 0;
    };
} // namespace compiler
