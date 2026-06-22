//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      Graphicモジュール設定
    struct RHIConfig {
        s32 frameBufferCount = 2;       //!< ダブルバッファリング
		bool enableBindless = false;    //!< Bindlessレンダリングを使用するか
    };

}