#pragma once
namespace compiler::x86 {
    enum class CC {
        Empty,
        Equal, //e
        NotEqual, //ne
        Less, //l
        LessOrEqual, //le
        Greater, //g
        GreaterOrEqual, //ge
        Below, //b
        BelowOrEqual, //be
        Above, //a
        AboveOrEqual, //ae
        Overflow, //o
        NoOverflow, //no
        Sign, //s
        NotSign, //ns
        ParityEven, // p
        ParityOdd // np
    };
}
