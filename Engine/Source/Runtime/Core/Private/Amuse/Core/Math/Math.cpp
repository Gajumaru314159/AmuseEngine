//***********************************************************
//! @file
//! @author Gajumaru
//***********************************************************
#include <Amuse/Core/Math/Math.h>
#include <cmath>

namespace Amuse {

    //! @brief 値が無効な数字か判定する
    bool Math::IsNaN(f32 value) {
        return std::isnan(value);
    }

    //! @brief 値が無限か判定する
    bool Math::IsInf(f32 value) {
        return std::isinf(value);
    }
}