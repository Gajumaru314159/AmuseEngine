//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Constants.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/BlendDesc.h>
#include <Amuse/RHI/Types/DepthStencilDesc.h>
#include <Amuse/RHI/Types/RasterizerDesc.h>
#include <Amuse/RHI/Types/Topology.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse {
	//! @brief		サンプリング定義
	//! 
	//! @details	描画速度に直結するため、アンチエイリアシングの使用を検討してください。
	//! @ingroup AmuseRHI
	struct SampleDesc {
		s32 count	=1;		//!< ピクセル当たりのマルチサンプル数
		s32 quality =0;		//!< 品質
	};

	//! @brief  レンダーターゲット・フォーマット・配列
	using RenderTargetFormatArray = FixedVector<TextureFormat, RENDER_TARGET_MAX>;

	//! @brief  パイプラインステート定義
	//! @ingroup AmuseRHI
	struct PipelineStateDesc {
		
		String					name;							//!< 名前

		RenderTargetFormatArray	colors;						//!< 描画先フォーマット
		Optional<TextureFormat>	depth; //!< 深度値

		Ref<RootSignature>		rootSignature;					//!< ルートシグネチャ
		Ref<VertexLayout>		vertexLayout;					//!< 頂点レイアウト

		Ref<Shader>				vs;								//!< 頂点シェーダ
		Ref<Shader>				gs;								//!< ジオメトリシェーダ
		Ref<Shader>				hs;								//!< ハルシェーダ
		Ref<Shader>				ds;								//!< ドメインシェーダ
		Ref<Shader>				ps;								//!< ピクセルシェーダ

		SampleDesc				sample;							//!< サンプル定義
		BlendDescList			blend;							//!< ブレンド定義
		RasterizerDesc			rasterizer;						//!< ラスタライズ定義
		DepthStencilDesc		depthStencil;					//!< デプス・ステンシル定義

		Topology				topology=Topology::TriangleList;//!< GeometryShaderでのトポロジー
		u32						sampleMask=~static_cast<u32>(0);//!< マルチレンダーターゲットの何枚目に書き込むか(下位ビットから)
	};

}
