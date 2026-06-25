//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
    //! @brief  ウィンドウ・モード
    //! @ingroup AmusePlatform
    enum class WindowMode {
        Window,     //!< ウィンドウ
        FullScreen, //!< フルスクリーン
        Borderless, //!< ボーダーレス(仮想フルスクリーン)
    };

    //! @brief  ウィンドウ生成設定
    //! @ingroup AmusePlatform
    struct WindowDesc {
        String      title = "NONAME";			//!< ウィンドウタイトル
        Vec2		clientSize{ 1280, 720};	    //!< クライアント領域のピクセルサイズ
        WindowMode	mode = WindowMode::Window;  //!< フルスクリーンモードで作成するかどうか
        bool		resizable = true;			//!< 可変ウィンドウとして作成するかどうか
		bool		show = false;				//!< ウィンドウ生成時に表示するかどうか
    };

}