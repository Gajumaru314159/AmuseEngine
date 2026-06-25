//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/RootSignatureDesc.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>
#include <Amuse/RHI/Types/CommandListDesc.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/RHI/Types/TextureDesc.h>
#include <Amuse/RHI/Types/RenderPassDesc.h>
#include <Amuse/RHI/Types/DescriptorLayoutDesc.h>
#include <Amuse/RHI/Types/VertexLayout.h>

namespace Amuse {
    //! @brief  DirectXの内部型変換
    //! @ingroup AmuseDirectX12RHI
    class TypeConverter {
    public:

        static D3D12_ROOT_SIGNATURE_FLAGS Convert(RootSignatureFlags value);
        static D3D12_SHADER_VISIBILITY Convert(ShaderStage value);
        static D3D12_FILTER_TYPE Convert(TextureFilter value);
        static D3D12_FILTER_TYPE Convert(MipFilter value);
        static D3D12_FILTER Convert(TextureFilter up, TextureFilter down, MipFilter mip, bool anisotropic);
        static UINT Convert(Anisotropy value);
        static D3D12_TEXTURE_ADDRESS_MODE Convert(TextureAddress value);
        static D3D12_FILL_MODE Convert(FillMode value);
        static D3D12_CULL_MODE Convert(CullMode value);
        static DXGI_FORMAT Convert(TextureFormat value, bool useTypeless = false);
        static DXGI_FORMAT ConvertDepthAsColor(TextureFormat value);
        static D3D12_BLEND Convert(BlendFactor value);
        static D3D12_BLEND_OP Convert(BlendOp value);
        static UINT8 Convert(ColorMask value);
        static D3D12_LOGIC_OP Convert(LogicOp value);
        static D3D12_PRIMITIVE_TOPOLOGY Convert(Topology value);
        static DXGI_FORMAT Convert(ElementType value,s32 dimension);
        static LPCSTR Convert(Semantic value);
        static D3D12_STENCIL_OP Convert(StencilOp value);
        static D3D12_COMPARISON_FUNC Convert(ComparisonFunc value);
        static D3D12_COMMAND_LIST_TYPE Convert(CommandListType value);
        static D3D12_RESOURCE_STATES Convert(BufferState value);
        static D3D12_RESOURCE_STATES Convert(TextureState value);
        static D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE Convert(RenderPassBeforeAccessType value);
        static D3D12_RENDER_PASS_ENDING_ACCESS_TYPE Convert(RenderPassAfterAccessType value);
        static D3D12_DESCRIPTOR_RANGE_TYPE Convert(BindingType value);

        static TextureFormat Convert(DXGI_FORMAT dxgi);


    };
}
