//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/System.h>
#include <Amuse/RHI/EmptyRHI.h>
#include <Amuse/RHI/GraphicObjectManager.h>
#include <Amuse/RHI/SystemResource.h>
#include <Amuse/Core/Utility/DI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      システムをServiceInjectorに登録
	void RegisterRHIService(ServiceInjector& injector) {
		injector.bind<GraphicObjectManager>();
		injector.bind<SystemResource>();
		injector.bind<EmptyRHI>().as<Device>();
	}

}