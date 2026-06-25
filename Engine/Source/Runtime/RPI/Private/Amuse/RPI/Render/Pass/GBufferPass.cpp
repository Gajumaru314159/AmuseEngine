//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Pass/GBufferPass.h>
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
namespace Amuse {

	EarlyZPass::EarlyZPass() {

	}

	EarlyZPass::Output EarlyZPass::render(FG& fg, RenderView& view, const Input& input)const {

		return fg.addPass<Output>(
			"EarlyZPass",
			[&](FGBuilder& builder, Output& output) {
				output.depth = builder.write(input.depth);
			},
			[&](const Output& output, FGResources& resources, Ref<CommandList>& cmdList) {
				if (auto feature = view.findFeature<MaterialRenderFeature>()) {

					MaterialBlockSet blocks(view);

					cmdList->pushMarker("EarlyZ");

					BeginPassParam param;
					param.depth = { resources.getTexture(output.depth), RenderPassBeforeAccessType::Clear, RenderPassAfterAccessType::Preserve };

					cmdList->beginRenderPass(param);

					feature->render(&view,cmdList, blocks, "EarlyZ");

					cmdList->endRenderPass();

					cmdList->popMarker();
				}
			}
		);

	}

	//--------------------------------

	OpaquePass::OpaquePass() {

	}

	OpaquePass::Output OpaquePass::render(FG& fg, RenderView& view, const Input& input)const {

		return fg.addPass<Output>(
			"Opaque",
			[&](FGBuilder& builder, Output& output) {
				output.albedo = builder.write(input.albedo);
				output.normal = builder.write(input.normal);
				output.params = builder.write(input.params);
				output.depth = builder.read(input.depth);
			},
			[&](const Output& output, FGResources& resources, Ref<CommandList>& cmdList) {
				if (auto feature = view.findFeature<MaterialRenderFeature>()) {

					cmdList->pushMarker("Opaque");

					BeginPassParam param;
					param.colors.emplace_back(resources.getTexture(output.albedo), RenderPassBeforeAccessType::Clear, RenderPassAfterAccessType::Preserve);
					param.colors.emplace_back(resources.getTexture(output.normal), RenderPassBeforeAccessType::Clear, RenderPassAfterAccessType::Preserve);
					param.colors.emplace_back(resources.getTexture(output.params), RenderPassBeforeAccessType::Clear, RenderPassAfterAccessType::Preserve);
					param.depth = { resources.getTexture(output.depth), RenderPassBeforeAccessType::Preserve, RenderPassAfterAccessType::Preserve };

					cmdList->beginRenderPass(param);

					MaterialBlockSet blocks(view);
					feature->render(&view,cmdList, blocks, "Opaque");

					cmdList->endRenderPass();

					cmdList->popMarker();
				}
			}
		);

	}

	//--------------------------------

	MaskedPass::MaskedPass() {

	}

	MaskedPass::Output MaskedPass::render(FG& fg, RenderView& view, const Input& input)const {

		return fg.addPass<Output>(
			"Masked",
			[&](FGBuilder& builder, Output& output) {
				output.albedo = builder.write(input.albedo);
				output.normal = builder.write(input.normal);
				output.params = builder.write(input.params);
				output.depth = builder.read(input.depth);
			},
			[&](const Output& output, FGResources& resources, Ref<CommandList>& cmdList) {
				if (auto feature = view.findFeature<MaterialRenderFeature>()) {

					cmdList->pushMarker("Masked");

					BeginPassParam param;
					param.colors.emplace_back(resources.getTexture(output.albedo), RenderPassBeforeAccessType::Preserve, RenderPassAfterAccessType::Preserve);
					param.colors.emplace_back(resources.getTexture(output.normal), RenderPassBeforeAccessType::Preserve, RenderPassAfterAccessType::Preserve);
					param.colors.emplace_back(resources.getTexture(output.params), RenderPassBeforeAccessType::Preserve, RenderPassAfterAccessType::Preserve);
					param.depth = { resources.getTexture(output.depth), RenderPassBeforeAccessType::Preserve, RenderPassAfterAccessType::Preserve };

					cmdList->beginRenderPass(param);

					MaterialBlockSet blocks(view);
					feature->render(&view,cmdList, blocks, "Masked");

					cmdList->endRenderPass();

					cmdList->popMarker();
				}
			}
		);

	}

}