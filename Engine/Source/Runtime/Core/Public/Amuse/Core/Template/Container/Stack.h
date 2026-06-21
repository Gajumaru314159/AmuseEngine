//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <stack>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>
#include <Amuse/Core/Template/Container/Deque.h>

namespace Amuse::Core {

    //! @brief スタック
    template <class T,class TContainer = Deque<T>>
    using Stack = std::stack<T, TContainer>;

}