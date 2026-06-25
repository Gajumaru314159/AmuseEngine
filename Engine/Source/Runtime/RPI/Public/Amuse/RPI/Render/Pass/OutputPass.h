//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPass.h>

namespace Amuse {

	//! @brief OutputPass のビュー別出力先情報
	//! @ingroup AmuseRPI
	struct OutputViewData {
		Ref<SwapChain>		swapchain;			//!< 描画先
		Ref<RenderTexture> texture;			//!< 描画先
		Rect                    rect{0,0,1,1};		//!< 描画範囲の矩形
		Viewport				viewport{0,0,1,1};	//!< 描画範囲

		//! @brief 出力先サイズを取得する
		Optional<Size> size()const;
	};

	//! @brief 最終カラーをスワップチェーンまたはテクスチャへ出力するパス
	//! @ingroup AmuseRPI
	class OutputPass : public RenderPass {
	public:
		//! @brief OutputPass の入力リソース
		struct Input {
			FGResource color; //!< カラー
		};
		//! @brief OutputPass の出力リソース
		//! @ingroup AmuseRPI
		struct Output : Input {
		};
	public:

		//! @brief OutputPass を FrameGraph に登録する
		Output render(FG& fg, RenderView& view, const Input& input) const;

	};

}
