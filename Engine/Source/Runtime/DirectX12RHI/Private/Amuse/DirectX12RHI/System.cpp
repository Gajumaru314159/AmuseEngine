//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/Platform/System.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      システムをServiceInjectorに登録
	void RegisterDirectX12RHIService(ServiceInjector& injector) {
		DirectX12Device::Inject(injector);
	}

}