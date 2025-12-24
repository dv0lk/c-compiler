#pragma once
#include <span>
#include <vector>
#include "function.hpp"

namespace compiler {
    template<typename InstrType>
    class Program {
    public:
        using function_t = Function<InstrType>;

        Program() = default;

        void add_function(function_t &&function) {
            functions_.emplace_back(std::move(function));
        }

        [[nodiscard]] bool empty() const { return functions_.empty(); }

        [[nodiscard]] std::vector<function_t> functions() { return functions_; }
        [[nodiscard]] std::vector<function_t> functions() const { return functions_; }

        [[nodiscard]] auto begin() { return functions_.begin(); }
        [[nodiscard]] auto end() { return functions_.end(); }

    private:
        std::vector<function_t> functions_;
    };
}
