//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPass.h>

namespace Amuse::RPI {

	struct OutputViewData {
		Ref<RHI::SwapChain>		swapchain;			//!< 描画先
		Ref<RHI::RenderTexture> texture;			//!< 描画先
		Rect                    rect{0,0,1,1};		//!< 描画範囲の矩形
		Viewport				viewport{0,0,1,1};	//!< 描画範囲

		Optional<Size> size()const;
	};

	class OutputPass : public RenderPass {
	public:
		struct Input {
			FGResource color;
		};
		struct Output : Input {
		};
	public:

		Output render(FG& fg, RenderView& view, const Input& input) const;

	};

}