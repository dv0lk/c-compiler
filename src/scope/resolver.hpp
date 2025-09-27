#pragma once
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

class Resolver {
public:
    int count = 0;
    std::vector<std::unordered_map<std::string, int> > scopes;

    Resolver() {
        begin_scope();
    }

    void begin_scope() {
        scopes.emplace_back();
    }

    void end_scope() {
        scopes.pop_back();
    }

    std::optional<int> declare(const std::string& name) {
        if (scopes.empty())
            return {};

        scopes.back()[name] = ++count;
        return count;
    }

    std::optional<int> resolve(const std::string& name) {
        for (const auto& scope : std::views::reverse(scopes)) {
            if (scope.contains(name)) {
                return scope.at(name);
            }
        }
        return {};
    }
};
