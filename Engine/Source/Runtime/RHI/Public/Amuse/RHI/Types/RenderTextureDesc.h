//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/TextureDesc.h>

namespace Amuse {
    //! @brief  クリアカラー
    //! @ingroup AmuseRHI
    struct ClearColor {
        Color color = Color::White; //!< クリアカラー
        f32 depth = 0.0f; //!< 深度値
        u32 stencil = 0; //!< ステンシル値
    };

    //! @brief  レンダーテクスチャ定義
    //! @ingroup AmuseRHI
    struct RenderTextureDesc {
        String          name; //!< 名前
        TextureFormat   format = TextureFormat::RGBA8;  //!< テクスチャフォーマット
        Size            size = { 0,0,0 };               //!< サイズ
        ClearColor      clear;                          //!< クリアカラー

        //! @brief 有効な設定か判定する
        bool isValid() const;
    };

}
