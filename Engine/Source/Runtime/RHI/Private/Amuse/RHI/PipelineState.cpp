//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/PipelineState.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  生成
	Ref<PipelineState> PipelineState::Create(const PipelineStateDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createPipelineState(desc);
		}
		return nullptr;
	}

}