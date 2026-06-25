//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <deque>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse {

    //! @brief Deque(二重終端キュー)
    template <class T, class TAlloc = Amuse::STLAllocator<T>>
    using Deque = std::deque<T, TAlloc>;

}