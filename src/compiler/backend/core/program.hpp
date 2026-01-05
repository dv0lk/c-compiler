#pragma once
#include "function.hpp"
#include <set>
#include <string>
#include <vector>

namespace compiler {
    template <typename InstrType>
    class Program {
    public:
        using function_t = Function<InstrType>;

        Program() = default;

        void add_function(function_t&& function) {
            functions_.emplace_back(std::move(function));
        }

        void add_extern(const std::string& name) {
            externs_.insert(name);
        }

        [[nodiscard]] const std::set<std::string>& externs() const {
            return externs_;
        }

        [[nodiscard]] bool empty() const {
            return functions_.empty();
        }

        [[nodiscard]] std::vector<function_t>& functions() {
            return functions_;
        }
        [[nodiscard]] const std::vector<function_t>& functions() const {
            return functions_;
        }

        [[nodiscard]] auto begin() {
            return functions_.begin();
        }
        [[nodiscard]] auto end() {
            return functions_.end();
        }

    private:
        std::vector<function_t> functions_;
        std::set<std::string> externs_;
    };
} // namespace compiler
