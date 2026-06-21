//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse::Core {

    //! @brief  コンパイル時定数
    template <typename T, T N> struct ConstValue {
        static constexpr T value = N; //!< 値
    };

}
