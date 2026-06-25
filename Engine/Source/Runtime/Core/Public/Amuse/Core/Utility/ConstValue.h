//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse {

    //! @brief  コンパイル時定数
    //! @ingroup AmuseCore
    template <typename T, T N> struct ConstValue {
        static constexpr T value = N; //!< 値
    };

}
