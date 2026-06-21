//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <list>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse::Core {

    //! @brief 双方向連結リスト
    template <class T, class TAlloc = Amuse::Core::STLAllocator<T>>
    using List = std::list<T, TAlloc>;

}