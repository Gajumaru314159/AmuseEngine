//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/Types/CommandListDesc.h>

namespace Amuse {
	//! @brief  生成
	Ref<CommandList> CommandList::Create(const CommandListDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createCommandList(desc);
		}
		return nullptr;
	}

}
