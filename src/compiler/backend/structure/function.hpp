#pragma once
#include <span>
#include <string>
#include <vector>

namespace compiler {
    template<typename InstrType>
    class Function {
    public:
        Function() = default;

        explicit Function(std::string function_name)
            : name_(std::move(function_name)) {
        }

        void add_param(std::string param_name) {
            params_.emplace_back(std::move(param_name));
        }

        void push_back(const InstrType& instr) {
            instructions_.push_back(instr);
        }

        void push_back(InstrType&& instr) {
            instructions_.push_back(std::move(instr));
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            instructions_.emplace_back(std::forward<Args>(args)...);
        }

        [[nodiscard]] const std::string& name() const { return name_; }

        [[nodiscard]] std::vector<std::string>& params() { return params_; }
        [[nodiscard]] const std::vector<std::string>& params() const { return params_; }

        [[nodiscard]] std::vector<InstrType>& instructions() { return instructions_; }
        [[nodiscard]] const std::vector<InstrType>& instructions() const { return instructions_; }
        [[nodiscard]] std::span<const InstrType> instructions_span() const { return instructions_; }

        [[nodiscard]] bool empty() const { return instructions_.empty(); }
        [[nodiscard]] size_t size() const { return instructions_.size(); }

        auto begin() { return instructions_.begin(); }
        auto end() { return instructions_.end(); }
        auto begin() const { return instructions_.begin(); }
        auto end() const { return instructions_.end(); }

    private:
        std::string name_;
        std::vector<InstrType> instructions_;
        std::vector<std::string> params_;
    };
}
