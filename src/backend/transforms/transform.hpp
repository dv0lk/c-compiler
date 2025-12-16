#pragma once

template<typename InstrType>
class Transform {
public:
    Transform() = default;

    virtual ~Transform() = default;

    [[nodiscard]] virtual bool run(InstrType& instructions) = 0;
};

