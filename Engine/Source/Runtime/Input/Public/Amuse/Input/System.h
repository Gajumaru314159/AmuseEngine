//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::Input {
    using namespace Amuse::Core;

    //! @brief      システムをServiceInjectorに登録
    void RegisterInputService(ServiceInjector&);

}