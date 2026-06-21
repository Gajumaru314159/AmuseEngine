//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <vector>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse::Core {

	//! @brief 動的配列
	template <class T, class TAlloc = STLAllocator<T>>
	using Vector = std::vector<T, TAlloc>;

}