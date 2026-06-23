//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPipeline.h>
#include <Amuse/RPI/Render/Pass/ImGuiPass.h>
#include <Amuse/RPI/Render/Pass/OutputPass.h>

namespace Amuse::RPI {

	class ImGuiRenderPipeline : public RenderPipeline {
	public:
		static Ref<ImGuiRenderPipeline> Create() {
			return new ImGuiRenderPipeline();
		}
	private:
		ImGuiRenderPipeline() {}
	public:
		void setup(RenderScene& scene, RenderFeatureSet& features) const override;
		void render(FG& fg, RenderView& view) const override;
	private:
		ImGuiPass m_imgui;
		OutputPass m_output;
	};

}