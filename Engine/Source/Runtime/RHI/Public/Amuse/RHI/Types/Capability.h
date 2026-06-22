//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  RHI機能
	struct RHIFeatures {
		bool computeQueue = false;					//!< Compute queue
		bool copyQueue = false;						//!< Copy queue
		bool indirectDraw = false;					//!< Indirect draw
		bool indirectDispatch = false;				//!< Indirect dispatch
		bool bindless = false;						//!< Bindless descriptor
		bool rayTracing = false;					//!< Ray tracing
		bool meshShader = false;					//!< Mesh shader
		bool variableRateShading = false;			//!< Variable rate shading
		bool conservativeRasterization = false;		//!< Conservative rasterization
		bool stagingTexture = false;				//!< Staging texture
	};

	//! @brief  フォーマット利用用途
	enum class FormatSupport {
		Texture,			//!< Textureとして利用可能
		RenderTarget,	//!< RenderTargetとして利用可能
		DepthStencil,	//!< DepthStencilとして利用可能
		ShaderResource,	//!< ShaderResourceとして利用可能
		UnorderedAccess,//!< UnorderedAccessとして利用可能
		Copy,			//!< Copy source/destとして利用可能
		Resolve,		//!< Resolve source/destとして利用可能
	};

}
