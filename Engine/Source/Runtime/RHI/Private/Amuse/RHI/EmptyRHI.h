//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/GraphicObjectManager.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      空RHI
	//! @details	Headlessビルドなどで使用する空のRHI
	class EmptyRHI : public Device {
	public:

		EmptyRHI(GraphicObjectManager& manager,RHIConfig* config)
			: Device(manager,config)
		{

		}

	};

}