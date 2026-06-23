//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPass.h>

namespace Amuse::RPI {

	//! @brief DeferredPass のビュー別データ
	struct DeferredData {
		Ref<Material> material; //!< 合成マテリアル
	};

	//! @brief GBuffer をライティングしてカラーへ合成する描画パス
	class DeferredPass : public RenderPass {
	public:
		//! @brief DeferredPass の入力リソース
		struct Input {
			FGResource albedo; //!< アルベド
			FGResource normal; //!< 法線
			FGResource params; //!< マテリアルパラメータ
			FGResource depth; //!< 深度
		};
		//! @brief DeferredPass の出力リソース
		struct Output : Input{
			FGResource color; //!< カラー
		};
	public:
		//! @brief DeferredPass を生成する
		DeferredPass();
		//! @brief DeferredPass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, const Input& input)const;
	private:
		Ref<Mesh> m_mesh;
	};

}
