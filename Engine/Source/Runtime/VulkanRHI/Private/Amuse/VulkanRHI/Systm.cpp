
//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/Platform/System.h>
#include <Amuse/VulkanRHI/System.h>
#include <Amuse/VulkanRHI/VulkanDevice.h>

namespace Amuse
{
	//@―---------------------------------------------------------------------------
	//! @brief      システムをServiceInjectorに登録
	//@―---------------------------------------------------------------------------
	void RegisterVulkanRHIService(ServiceInjector& injector) {
		injector.bind<VulkanDevice>().as<Device>();
		RegisterPlatformService(injector);
	}


}