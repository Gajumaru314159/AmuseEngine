//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Pipeline/UniversalRenderPipeline.h>
#include <Amuse/RPI/Render/Feature/CameraRenderFeature.h>
#include <Amuse/RPI/Render/Feature/ImGuiRenderFeature.h>
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
#include <Amuse/RPI/Render/Feature/LightRenderFeature.h>
#include <Amuse/RPI/FrameGraph/FG.h>
#include "Amuse/RPI/Render/Feature/MeshRenderFeature.h"

namespace Amuse::RPI {

	void UniversalRenderPipeline::setup(RenderScene& scene, RenderFeatureSet& features) const {
		features.add<MaterialRenderFeature>(scene);
		features.add<ImGuiRenderFeature>(scene);
		features.add<CameraRenderFeature>(scene);
		features.add<PointLightRenderFeature>();
	}

	void UniversalRenderPipeline::render(FG& fg, RenderScene& scene) const {
		if (auto light = scene.findFeature<PointLightRenderFeature>()) {
			light->render(fg,scene);
		}
		if (auto meshRF = scene.findFeature<MeshRenderFeature>()) {
			MeshRenderFeature::UpdateContext context{};
			context.scene = &scene;
			meshRF->update(context);
		}
	}

	void UniversalRenderPipeline::render(FG& fg, RenderView& view) const {

		auto size = view.get<OutputViewData>().size();

		if (!size) return;

		// リソースセットアップ6

		struct Resources {
			FGResource albedo;
			FGResource normal;
			FGResource params;
			FGResource depth;
		};
		
		auto resource = fg.addPass<Resources>(
			"ResourceSetup",
			[&](FGBuilder& builder, Resources& resources) {
				RHI::RenderTextureDesc desc;
				desc.size = *size;
				{
					desc.name = "Albedo";
					desc.format = RHI::TextureFormat::RGBA8;
					desc.clear.color = Color::Black;
					resources.albedo = builder.write(builder.create(desc));
				}
				{
					desc.name = "Normal";
					desc.format = RHI::TextureFormat::RGBA8;
					desc.clear.color = Color::Normal;
					resources.normal = builder.write(builder.create(desc));
				}
				{
					desc.name = "Params";
					desc.format = RHI::TextureFormat::RGBA8;
					desc.clear.color = Color::Black;
					resources.params = builder.write(builder.create(desc));
				}
				{
					desc.name = "Depth";
					desc.format = RHI::TextureFormat::D32;
					resources.depth = builder.write(builder.create(desc));
				}
			}
		);

		// TODO CullBufferを生成
		// * RenderableのBoundsをGPUに転送
		// * FrustumCullingをGPUで実行
		auto gbuffer = m_gbuffer.render(fg, view, { resource.albedo, resource.normal, resource.params, resource.depth });
		auto deferred = m_deferred.render(fg, view, { gbuffer.albedo,gbuffer.normal,gbuffer.params, gbuffer.depth });
		auto imgui = m_imgui.render(fg, view, { deferred.color });

		bool useImGui = true;

		auto output = m_output.render(fg, view, { useImGui ? imgui.color : deferred.color });
	}

}
