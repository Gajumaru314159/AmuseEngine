//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <queue>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>
#include <Amuse/Core/Template/Container/Deque.h>
#include <Amuse/Core/Template/Container/Vector.h>

namespace Amuse::Core {

    //! @brief キュー
    template <class T, class TConatiner = Deque<T>>
    using Queue = std::queue<T, TConatiner>;

    //! @brief 優先度付きキュー
    template <class T, class TContainer = Vector<T>, class TPr = std::less<typename TContainer::value_type>>
    using PriorityQueue = std::priority_queue<T,TContainer,TPr>;

}