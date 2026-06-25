//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Math/Vector3.h>

namespace Amuse {

    //! @brief  線分
    //! @ingroup AmuseCore
    struct Line {
    public:

        //! @brief  デフォルトコンストラクタ(初期化なし)
        Line() = default;


        //! @brief  コンストラクタ(2点指定)
        //! 
        //! @param p0 直線上の点1
        //! @param p1 直線上の点2
        constexpr Line(Vec3 p0, Vec3 p1)noexcept;

    public:

        Vec3 p0;    //!< 始点
        Vec3 p1;    //!< 半直線状のもう1点

    };






    //===============================================================
    // インライン関数
    //===============================================================
    //! @cond

    //! @brief  コンストラクタ(2点指定)
    //! 
    //! @param p0 直線上の点1
    //! @param p1 直線上の点2
    constexpr Line::Line(Vec3 p0, Vec3 p1)noexcept
        : p0(p0),p1(p1)
    {
    }

    //! @endcond
}