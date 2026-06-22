//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

#pragma region Enum

    //! @brief  コマンドリスト・タイプ
    enum class CommandListType :u32 {
        Graphic,    //!< グラフィック
        Compute,    //!< コンピュート
        Copy
        // VideoDecode
        // VideoProcess
        // VideoEncode
    };

#pragma endregion

    //! @brief  コマンドリスト定義
    struct CommandListDesc{
        String			name;	//!< 名前
        CommandListType type;   //!< 種類
    };

}