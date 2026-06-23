//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>

namespace Amuse::RPI {

	//! @brief      ImGui描画機能
	class ImGuiRenderFeature : public RenderFeature {
	public:

		AMUSE_RTTI();

		ImGuiRenderFeature(RenderScene& scene);
		virtual ~ImGuiRenderFeature();

	};

}