//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  テクスチャ・タイプ
    enum class TextureType {
        Texture1D,          //!< 1Dテクスチャ
        Texture2D,          //!< 2Dテクスチャ
        Texture3D,          //!< 3Dテクスチャ
        Cube,               //!< キューブテクスチャ
    };
    // TODO RenderTarget/DepthStencilを別管轄にする

    //! @brief      テクスチャ状態
    //! 
    //! @details    D3D12_RESOURCE_STATE VkImageLayout 参照
    //!             | TextureState                      | D3D12_RESOURCE_STATE              | VkImageLayout                 |
    //!             |-----------------------------------|-----------------------------------|-------------------------------|
    //!             | Common                            | COMMON                            | -                             |
    //!             | ShaderResource                    | PIXEL_SHADER_RESOURCE             | SHADER_READ_ONLY              |
    //!             | UnorderedAccess                   | UNORDERED_ACCESS                  | GENERAL                       |
    //!             | RenderTarget                      | RENDER_TARGET                     | COLOR_ATTACHMENT              |
    //!             | DepthRead                         | DEPTH_READ                        | DEPTH_STENCIL_READ_ONLY       |
    //!             | DepthWrite                        | DEPTH_WRITE                       | DEPTH_STENCIL_ATTACHMENT      |
    //!             | CopyDest                          | COPY_DEST                         | TRANSFER_SRC                  |
    //!             | CopySource                        | COPY_SOURCE                       | TRANSFER_DST                  |
    //!             | Present                           | PRESENT                           | PRESENT_SRC_KHR               |
    enum class TextureState {
        Unknown,            //!< 現在状態をRHIが知らない
        Common,
        ShaderResource,
        UnorderedAccess,
        RenderTarget,
        DepthRead,
        DepthWrite,
        CopyDest,
        CopySource,
        ResolveSource,
        ResolveDest,
        Present,
    };


    enum class TextureFlag {
        ShaderResource = get_bit(0),	//!< シェーダでバインド許可
        UnorderedAccess = get_bit(1),   //!< UnorderedAccessのバインド許可
	};
    using TextureFlags = BitFlags<TextureFlag>;


    enum class TextureViewType {
        Texture,
        RWTexture,
    };

    //! @brief D3D12_SHADER_RESOURCE_VIEW_DESCまたはD3D12_UNORDERED_ACCESS_VIEW_DESC相当
    struct TextureViewDesc {
        Ref<Texture> base; //!< base
        TextureViewType type = TextureViewType::Texture; //!< 種別
        s32 firstMip = 0;      //!< 先頭ミップ
        s32 mipLevels = 0;     //!< ミップ数。0以下の場合は残りすべて
        s32 firstArray = 0;    //!< 先頭配列要素。Cubeの場合は先頭Cube
        s32 arrayCount = 0;    //!< 配列要素数。0以下の場合は残りすべて
        s32 planeSlice = 0;    //!< プレーン番号
    };


    //! @brief  テクスチャ定義
    struct TextureDesc {
        String          name; //!< 名前
        TextureType     type        = TextureType::Texture2D;   //!< テクスチャタイプ
        TextureFormat   format      = TextureFormat::RGBA8;     //!< テクスチャフォーマット
        Size            size        = { 1,1,1 };                //!< サイズ
        s32             arrayNum    = 0;                        //!< テクスチャ配列の要素数 (Texture3Dでは0にしてください)
        s32             mipLevels   = 0;                        //!< ミップ生成レベル (0の場合sizeから自動計算されます)
		TextureFlags    flags       = TextureFlag::ShaderResource;            //!< フラグ

        //! @brief 有効な設定か判定する
        bool isValid() const;
    };

}
