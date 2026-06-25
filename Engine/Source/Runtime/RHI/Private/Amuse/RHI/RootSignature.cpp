//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/RootSignature.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse {
	//! @brief  生成
	Ref<RootSignature> RootSignature::Create(const RootSignatureDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createRootSignature(desc);
		}
		return nullptr;
	}

}