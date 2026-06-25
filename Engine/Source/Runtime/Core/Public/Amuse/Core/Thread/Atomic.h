//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <atomic>

namespace Amuse {

    //! @brief  アトミック型
    template<typename T>
    using Atomic = std::atomic<T>;

}