//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

    //! @brief      視錐台
    //! @ingroup AmuseCore
    struct Frustum {
        f32 left;	//!< 近投影面の左座標
        f32 right;	//!< 近投影面の右座標
        f32 bottom;	//!< 近投影面の下座標
        f32 top;	//!< 近投影面の上座標
        f32 zNear;	//!< ニアクリッピング面
        f32 zFar;	//!< ファークリッピング面
    };

}