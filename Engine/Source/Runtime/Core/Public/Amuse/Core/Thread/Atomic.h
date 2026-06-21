//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <atomic>

namespace Amuse::Core {

    //! @brief  アトミック型
    template<typename T>
    using Atomic = std::atomic<T>;

}