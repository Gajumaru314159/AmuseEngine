//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <deque>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse::Core {

    //! @brief Deque(二重終端キュー)
    template <class T, class TAlloc = Amuse::Core::STLAllocator<T>>
    using Deque = std::deque<T, TAlloc>;

}