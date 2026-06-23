//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>

namespace Amuse::RPI {

	class RenderPipeline : public RefObject {
	public:
		AMUSE_RTTI();

		~RenderPipeline() override = default;

		//! @brief		RenderSceneのセットアップを行う
		//! @details	RenderSceneにRenderPipelineが設定された時に実行されます。
		//!				RenderSceneに対して必要なRenderFeatureを登録します。
		virtual void setup(RenderScene& scene,RenderFeatureSet& features) const {}

		//! @brief		RenderPipeline毎の描画処理を行う
		virtual void render(FG& fg, RenderScene& scene) const {}

		//! @brief		RenderView毎の描画処理を行う 
		virtual void render(FG& fg, RenderView& view) const {}

	};


}