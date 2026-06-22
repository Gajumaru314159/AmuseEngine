//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Utility/Ref.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	// 列挙型
	enum class PresetTexture;

	// 構造体
	struct SetDescriptorTableParam;
	struct DrawParam;
	struct DrawIndexedParam;
	struct DispatchParam;
	struct IndirectCommandParam;
	struct TextureBarrier;
	struct BufferBarrier;
	struct GraphicFileQueueDesc;
	struct ShaderCompileDesc;
	struct RootSignatureDesc;
	struct PipelineStateDesc;
	struct ComputePipelineStateDesc;
	struct BindingSlot;
	struct VertexLayoutDesc;

	// クラス
	class Device;
	class Buffer;
	class CommandList;
	class EventQuery;
	class TimerQuery;
	class Fence;
	class DescriptorTable;
	class DescriptorLayout;
	class SwapChain;
	class PipelineState;
	class ComputePipelineState;
	class RenderTexture;
	class RootSignature;
	class Shader;
	class Texture;
	class Sampler;
	class VertexLayout;

	class GraphicFileHandle;
	class GraphicFileEvent;
	class GraphicFileQueue;

	// 内部クラス
	class GraphicObjectManager;

}
