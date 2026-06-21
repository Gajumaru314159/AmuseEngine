//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <forward_list>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse::Core {

    //! @brief 単連結リスト
    template <class T, class TAlloc = Amuse::Core::STLAllocator<T>>
    using ForwardList = std::forward_list<T, TAlloc>;

}