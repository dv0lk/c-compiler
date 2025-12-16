#pragma once
#include <span>
#include <vector>
#include "function.hpp"

namespace compiler::base {
    template<typename InstrType>
    class Program {
    private:
        std::vector<Function<InstrType> > functions_;

    public:
        Program() = default;

        void add_function(Function<InstrType> &&function) {
            functions_.emplace_back(std::move(function));
        }

        [[nodiscard]] std::span<Function<InstrType> > functions() {
            return functions_;
        }

        [[nodiscard]] std::span<const Function<InstrType>> functions() const {
            return functions_;
        }

        [[nodiscard]] bool empty() const {
            return functions_.empty();
        }

        [[nodiscard]] auto begin() {
            return functions_.begin();
        }

        [[nodiscard]] auto begin() const {
            return functions_.begin();
        }

        [[nodiscard]] auto end() {
            return functions_.end();
        }

        [[nodiscard]] auto end() const {
            return functions_.end();
        }
    };
}
