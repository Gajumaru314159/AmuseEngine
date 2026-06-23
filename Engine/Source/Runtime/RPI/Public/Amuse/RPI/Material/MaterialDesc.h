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

    struct ShaderSet {
		s32                             quality;

        // PipelineState周りの必須情報
        Ref<RHI::Shader> 		        vs;
        Ref<RHI::Shader> 		        ps;
        Vector<InputLayout>		        inputLayout;
        RHI::RenderTargetFormatArray    colors;
        Optional<RHI::TextureFormat>	depth;
		RHI::SampleDesc		            sample;
		RHI::BlendDescList		        blend;
        RHI::BlendDescList		        blends;
        RHI::RasterizerDesc		        rasterizer;
        RHI::DepthStencilDesc	        depthStencil;
    };

    struct MaterialPass {
		Vector<ShaderSet>   qualities;
    };

    using MaterialPassMap = Map<String, MaterialPass,std::less<>>;

    //! @brief  マテリアル定義
    struct MaterialDesc{
        String          name;
        
        Vector<String>  textures;
        Vector<String>  buffers;
        Vector<String>  matrices;
        Vector<String>  vectors;
        Vector<String>  scalars;
        Vector<String>  integers;

        MaterialPassMap	passes;
    };

}
