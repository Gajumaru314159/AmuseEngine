//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <set>
#include <unordered_set>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>

namespace Amuse::Core {

	//! @brief 順序あり集合
	template <class T, class TPr = std::less<T>, class TAlloc = Amuse::Core::STLAllocator<T>>
	using Set = std::set<T, TPr, TAlloc>;

	//! @brief 集合
	template <class T, class THasher = std::hash<T>, class TKeyEq = std::equal_to<T>, class TAlloc = Amuse::Core::STLAllocator<T>>
	using HashSet = std::unordered_set<T, THasher, TKeyEq, TAlloc>;

	//! @brief 複数順序あり集合
	template <class T, class TPr = std::less<T>, class TAlloc = Amuse::Core::STLAllocator<T>>
	using MultiSet = std::multiset<T, TPr, TAlloc>;

	//! @brief 複数集合
	template <class T, class THasher = std::hash<T>, class TKeyEq = std::equal_to<T>, class TAlloc = Amuse::Core::STLAllocator<T>>
	using HashMultiSet = std::unordered_multiset<T, THasher, TKeyEq, TAlloc>;

}