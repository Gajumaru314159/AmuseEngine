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

namespace Amuse {

    //! @brief  頂点属性
    //! @see    VertexLayout
    //! @ingroup AmuseRPI
    struct InputLayout {

        Semantic	    semantic;	//!< セマンティクス
        ElementType	type;		//!< コンポーネント型
        s32			        dimention;	//!< 次元数
        s32			        index;		//!< セマンティクス内インデックス

    public:

        //! @brief      コンストラクタ
        InputLayout() = default;

        //! @brief      コンストラクタ
        InputLayout(Semantic semantic, ElementType type, s32 dimention = 1, s32 index = 0)
            :semantic(semantic), type(type), dimention(dimention), index(index) {}

    };

    //! @brief マテリアルパスの品質別シェーダ設定
    //! @ingroup AmuseRPI
    struct ShaderSet {
		s32                             quality; //!< 品質値

        // PipelineState周りの必須情報
        Ref<Shader> 		        vs; //!< 頂点シェーダ
        Ref<Shader> 		        ps; //!< ピクセルシェーダ
        Vector<InputLayout>		        inputLayout; //!< 入力レイアウト
        RenderTargetFormatArray    colors; //!< カラー出力フォーマット
        Optional<TextureFormat>	depth; //!< 深度フォーマット
		SampleDesc		            sample; //!< サンプル設定
		BlendDescList		        blend; //!< ブレンド設定
        BlendDescList		        blends; //!< 複数ブレンド設定
        RasterizerDesc		        rasterizer; //!< ラスタライザ設定
        DepthStencilDesc	        depthStencil; //!< 深度ステンシル設定
    };

    //! @brief マテリアルの描画パス設定
    //! @ingroup AmuseRPI
    struct MaterialPass {
		Vector<ShaderSet>   qualities; //!< 品質別シェーダ設定
    };

    using MaterialPassMap = Map<String, MaterialPass,std::less<>>;

    //! @brief  マテリアル定義
    //! @ingroup AmuseRPI
    struct MaterialShaderDesc{
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
