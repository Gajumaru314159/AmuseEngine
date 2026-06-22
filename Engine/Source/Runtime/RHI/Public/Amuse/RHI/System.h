//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      システムをServiceInjectorに登録
	void RegisterRHIService(ServiceInjector&);

}