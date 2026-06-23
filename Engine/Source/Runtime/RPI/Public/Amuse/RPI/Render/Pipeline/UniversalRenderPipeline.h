//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPipeline.h>
#include <Amuse/RPI/Render/Pass/GBufferPass.h>
#include <Amuse/RPI/Render/Pass/DeferredPass.h>
#include <Amuse/RPI/Render/Pass/ImGuiPass.h>
#include <Amuse/RPI/Render/Pass/OutputPass.h>

namespace Amuse::RPI {

	class UniversalRenderPipeline : public RenderPipeline {
	public:
		static Ref<UniversalRenderPipeline> Create() {
			return new UniversalRenderPipeline();
		}
	private:
		UniversalRenderPipeline() {}
	public:
		void setup(RenderScene& scene, RenderFeatureSet& features) const override;
		void render(FG& fg, RenderScene& scene) const override;
		void render(FG& fg, RenderView& view) const override;
	private:
		// Global;

		// Local
		GBufferPass m_gbuffer;
		DeferredPass m_deferred;
		ImGuiPass m_imgui;
		OutputPass m_output;
	};

}