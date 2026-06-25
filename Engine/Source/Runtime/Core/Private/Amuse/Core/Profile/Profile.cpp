//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Profile/Profile.h>
#include <tracy/TracyC.h>

namespace Amuse {

    void BeginProfile() {
#if TRACY_MANUAL_LIFETIME
        ___tracy_startup_profiler();
#endif
    }

    void EndProfile() {
#if TRACY_MANUAL_LIFETIME
        ___tracy_shutdown_profiler();
#endif
    }

}