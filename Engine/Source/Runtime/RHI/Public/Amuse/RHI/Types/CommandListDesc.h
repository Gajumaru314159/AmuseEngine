//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
#pragma region Enum

    //! @brief  コマンドリスト・タイプ
    //! @ingroup AmuseRHI
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
    //! @ingroup AmuseRHI
    struct CommandListDesc{
        String			name;	//!< 名前
        CommandListType type;   //!< 種類
    };

}