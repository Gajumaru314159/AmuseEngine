//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/FrameGraph/FGResource.h>

namespace Amuse::RPI {

	//! @brief RenderPipeline で使用する描画パス基底クラス
	class RenderPass {
	public:
		//! @brief デストラクタ
		virtual ~RenderPass() = default;
	};

}
