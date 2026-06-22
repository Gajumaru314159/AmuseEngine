//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/TextureFormat.h>
#include <Amuse/RHI/Types/RootSignatureDesc.h>
#include <Amuse/RHI/Types/DescriptorLayoutDesc.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>
#include <Amuse/RHI/Types/TextureDesc.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/RHI/Types/RenderPassDesc.h>
#include <Amuse/RHI/Types/VertexLayout.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	class TypeConverter {
	public:
		static vk::ImageType Convert(TextureType value);
		static vk::AccessFlags Convert(BufferState state);
		static vk::PrimitiveTopology Convert(Topology value);
		static vk::Format Convert(ElementType type, s32 dimension);
		static s32 Convert(VertexInputRate value);
		static vk::PolygonMode Convert(FillMode value);
		static vk::CullModeFlags Convert(CullMode value);
		static vk::CompareOp Convert(ComparisonFunc value);
		static vk::BlendFactor Convert(BlendFactor value);
		static vk::BlendOp Convert(BlendOp value);
		static vk::StencilOp Convert(StencilOp value);
		static vk::ColorComponentFlags Convert(ColorMask value);
		static vk::Format Convert(TextureFormat value);
		static vk::ImageLayout Convert(TextureState value);
		static vk::DescriptorType Convert(BindingType value);
		static vk::Filter Convert(TextureFilter filter);
		static vk::SamplerMipmapMode Convert(MipFilter mipFilter);
		static vk::SamplerAddressMode Convert(TextureAddress address);
		static f32 Convert(Anisotropy anisotropy);
		static vk::AttachmentLoadOp Convert(RenderPassBeforeAccessType type);
		static vk::AttachmentStoreOp Convert(RenderPassAfterAccessType type);

		static TextureFormat Convert(vk::Format value);
	};

}
