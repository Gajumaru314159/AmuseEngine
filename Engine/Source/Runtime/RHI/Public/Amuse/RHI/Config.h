//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
    //! @brief      Graphicモジュール設定
    //! @ingroup AmuseRHI
    struct RHIConfig {
        s32 frameBufferCount = 2;       //!< ダブルバッファリング
		bool enableBindless = false;    //!< Bindlessレンダリングを使用するか
    };

}