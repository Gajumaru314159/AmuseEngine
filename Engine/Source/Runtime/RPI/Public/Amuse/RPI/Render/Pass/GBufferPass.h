//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPass.h>

namespace Amuse {

	//! @brief 深度のみを先行描画するパス
	//! @ingroup AmuseRPI
	class EarlyZPass : public RenderPass {
	public:
		//! @brief EarlyZPass の入力リソース
		struct Input {
			FGResource depth; //!< 深度
		};
		//! @brief EarlyZPass の出力リソース
		//! @ingroup AmuseRPI
		struct Output {
			FGResource depth; //!< 深度
		};
	public:
		//! @brief EarlyZPass を生成する
		EarlyZPass();
		//! @brief EarlyZPass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, const Input& input)const;
	};

	//! @brief 不透明メッシュを GBuffer へ描画するパス
	//! @ingroup AmuseRPI
	class OpaquePass : public RenderPass {
	public:
		//! @brief OpaquePass の入力リソース
		struct Input {
			FGResource albedo; //!< アルベド
			FGResource normal; //!< 法線
			FGResource params; //!< マテリアルパラメータ
			FGResource depth; //!< 深度
		};
		//! @brief OpaquePass の出力リソース
		//! @ingroup AmuseRPI
		struct Output : Input{
		};
	public:
		//! @brief OpaquePass を生成する
		OpaquePass();
		//! @brief OpaquePass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, const Input& input)const;
	};

	//! @brief マスク付きメッシュを GBuffer へ描画するパス
	//! @ingroup AmuseRPI
	class MaskedPass : public RenderPass {
	public:
		//! @brief MaskedPass の入力リソース
		struct Input {
			FGResource albedo; //!< アルベド
			FGResource normal; //!< 法線
			FGResource params; //!< マテリアルパラメータ
			FGResource depth; //!< 深度
		};
		//! @brief MaskedPass の出力リソース
		//! @ingroup AmuseRPI
		struct Output : Input{
		};
	public:
		//! @brief MaskedPass を生成する
		MaskedPass();
		//! @brief MaskedPass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, const Input& input)const;
	};


	//! @brief GBuffer 生成に必要なパスをまとめた描画パス
	//! @ingroup AmuseRPI
	class GBufferPass : public RenderPass {
	public:
		//! @brief GBufferPass の入力リソース
		struct Input {
			FGResource albedo; //!< アルベド
			FGResource normal; //!< 法線
			FGResource params; //!< マテリアルパラメータ
			FGResource depth; //!< 深度
		};
		//! @brief GBufferPass の出力リソース
		//! @ingroup AmuseRPI
		struct Output : Input{
		};
	public:
		//! @brief GBufferPass を生成する
		GBufferPass() {}
		//! @brief GBufferPass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, Input input)const {
			auto earlyZ = m_earlyZ.render(fg, view, { input.depth });
			auto opaque = m_opaque.render(fg, view, { input.albedo , input.normal, input.params, earlyZ.depth });
			auto masked = m_masked.render(fg, view, { opaque.albedo , opaque.normal, opaque.params, opaque.depth });
			return { masked.albedo,masked.normal, masked.params, masked.depth };
		}
	private:
		EarlyZPass m_earlyZ;
		OpaquePass m_opaque;
		MaskedPass m_masked;
	};

}
