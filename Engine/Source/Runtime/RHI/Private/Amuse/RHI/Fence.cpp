//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Fence.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  フェンスを作成
	Ref<Fence> Fence::Create(const FenceDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createFence(desc);
		}
		return nullptr;
	}

}
