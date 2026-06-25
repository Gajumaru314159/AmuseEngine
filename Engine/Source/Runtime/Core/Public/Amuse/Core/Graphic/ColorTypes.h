//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

    //! @brief		色空間列挙型
    //! @ingroup AmuseCore
    enum class GammaSpace :u8 {
        Liner,      //!< 線形
        sRGB        //!< ガンマ
    };


    //! @brief		色空間列挙型
    //! @ingroup AmuseCore
    enum class ColorCodeFormat :u8 {
        ARGB,
        ABGR,
        RGBA,
        BGRA,
        Default = ARGB
    };

}