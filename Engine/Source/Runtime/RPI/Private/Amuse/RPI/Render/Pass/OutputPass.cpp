//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Pass/OutputPass.h>
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RHI/SwapChain.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
namespace Amuse {

	Optional<Size> OutputViewData::size() const {
		if (swapchain) return swapchain->getDesc().size;
		if (texture) return texture->size();
		return std::nullopt;
	}

	OutputPass::Output OutputPass::render(FG& fg, RenderView& view, const Input& input) const {

		auto& data = view.get<OutputViewData>();
		auto& material = view.get<MaterialViewData>();

		material.block.setMatrix("MatrixV", Matrix::Identity);

		return fg.addPass<Output>(
			"CameraPass",
			[&](FGBuilder& builder, Output& output) {
				output.color = builder.read(input.color);
				builder.setSideEffect();
			},
			[&](const Output& output, FGResources& resources, Ref<CommandList>& cmdList) {
				auto albedo = resources.getTexture(output.color);

				if (data.swapchain) {
					cmdList->applySwapChain(data.swapchain, albedo);
				}
				else if (data.texture) {
					// TODO コピー処理
					AMUSE_NOTIMPLEMENTED();
				}

			}
		);

	}
}