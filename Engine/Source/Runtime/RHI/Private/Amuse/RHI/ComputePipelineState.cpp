//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/ComputePipelineState.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  生成
	Ref<ComputePipelineState> ComputePipelineState::Create(const ComputePipelineStateDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createComputePipelineState(desc);
		}
		return nullptr;
	}

}