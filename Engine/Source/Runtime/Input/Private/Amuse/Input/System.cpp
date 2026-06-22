//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Input/System.h>
#include <Amuse/Input/InputManager.h>
#include <Amuse/Platform/System.h>
#include <Amuse/Core/Utility/DI.h>

namespace Amuse::Input {
    using namespace Amuse::Core;

    //! @brief      システムをServiceInjectorに登録
    void RegisterInputService(ServiceInjector& injector) {
        injector.bind<InputModule>();
        Platform::RegisterPlatformService(injector);
    }

}