//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RPI/Render/Pipeline/ImGuiRenderPipeline.h>
#include <Amuse/RPI/Render/Feature/CameraRenderFeature.h>
#include <Amuse/RPI/Render/Feature/ImGuiRenderFeature.h>
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>

namespace Amuse::RPI {

	void ImGuiRenderPipeline::setup(RenderScene& scene, RenderFeatureSet& features) const {
		features.add<MaterialRenderFeature>(scene);
		features.add<ImGuiRenderFeature>(scene);
		features.add<CameraRenderFeature>(scene);
	}

	void ImGuiRenderPipeline::render(FG& fg, RenderView& view) const {

		auto size = view.get<OutputViewData>().size();

		if (!size) return;

		// リソースセットアップ

		struct Resources {
			FGResource color;
		};

		auto resources = fg.addPass<Resources>(
			"ResourceSetup",
			[&](FGBuilder& builder, Resources& resources) {
				RHI::RenderTextureDesc desc;
				desc.name = "Color";
				desc.size = *size;
				desc.format = RHI::TextureFormat::RGBA8;
				desc.clear.color = Color::Black;
				resources.color = builder.write(builder.create(desc));
			}
		);

		auto imgui = m_imgui.render(fg, view, { resources.color});
		auto output = m_output.render(fg, view, { imgui.color });
	}

}