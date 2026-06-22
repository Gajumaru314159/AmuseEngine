//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

#include <Amuse/Platform/Window.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      スワップチェーン定義
    struct SwapChainDesc {
        String              name;                       //!< 名前
        Ref<Platform::Window> window;                    //!< 描画ウィンドウ
        f32                 refreshRate=60.0f;          //!< リフレッシュ・レート
        Size                size = {0,0};               //!< バックバッファのサイズ(0の場合ウィンドウサイズを使用)
        s32                 bufferCount=2;              //!< バックバッファの数
        bool                vsync=false;                //!< VSyncが有効か
        bool                hdr=false;                  //!< HDRが有効か
    };

}
