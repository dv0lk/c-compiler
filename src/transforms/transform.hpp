#pragma once

template<typename InstructionType>
class Transform {
public:
    Transform() = default;

    virtual ~Transform() = default;

    [[nodiscard]] virtual bool run(InstructionType& instructions) = 0;
};

