//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/FrameGraph/FGResource.h>

namespace Amuse::RPI {

	class RenderPass {
	public:
		virtual ~RenderPass() = default;
	};

}