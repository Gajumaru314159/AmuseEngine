//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Constants.h>
#include <Amuse/RHI/Shader.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>
#include <Amuse/RHI/Types/VertexLayout.h>

namespace Amuse::RPI {

    //! @brief  頂点属性
    //! @see    VertexLayout
    struct InputLayout {

        RHI::Semantic	    semantic;	//!< セマンティクス
        RHI::ElementType	type;		//!< コンポーネント型
        s32			        dimention;	//!< 次元数
        s32			        index;		//!< セマンティクス内インデックス

    public:

        //! @brief      コンストラクタ
        InputLayout() = default;

        //! @brief      コンストラクタ
        InputLayout(RHI::Semantic semantic, RHI::ElementType type, s32 dimention = 1, s32 index = 0)
            :semantic(semantic), type(type), dimention(dimention), index(index) {}

    };

    //! @brief マテリアルパスの品質別シェーダ設定
    struct ShaderSet {
		s32                             quality; //!< 品質値

        // PipelineState周りの必須情報
        Ref<RHI::Shader> 		        vs; //!< 頂点シェーダ
        Ref<RHI::Shader> 		        ps; //!< ピクセルシェーダ
        Vector<InputLayout>		        inputLayout; //!< 入力レイアウト
        RHI::RenderTargetFormatArray    colors; //!< カラー出力フォーマット
        Optional<RHI::TextureFormat>	depth; //!< 深度フォーマット
		RHI::SampleDesc		            sample; //!< サンプル設定
		RHI::BlendDescList		        blend; //!< ブレンド設定
        RHI::BlendDescList		        blends; //!< 複数ブレンド設定
        RHI::RasterizerDesc		        rasterizer; //!< ラスタライザ設定
        RHI::DepthStencilDesc	        depthStencil; //!< 深度ステンシル設定
    };

    //! @brief マテリアルの描画パス設定
    struct MaterialPass {
		Vector<ShaderSet>   qualities; //!< 品質別シェーダ設定
    };

    using MaterialPassMap = Map<String, MaterialPass,std::less<>>;

    //! @brief  マテリアル定義
    struct MaterialDesc{
        String          name; //!< マテリアル名
        
        Vector<String>  textures; //!< テクスチャ名
        Vector<String>  buffers; //!< バッファ名
        Vector<String>  matrices; //!< 行列プロパティ名
        Vector<String>  vectors; //!< ベクトルプロパティ名
        Vector<String>  scalars; //!< スカラー名
        Vector<String>  integers; //!< 整数名

        MaterialPassMap	passes; //!< 描画パス設定
    };

}
