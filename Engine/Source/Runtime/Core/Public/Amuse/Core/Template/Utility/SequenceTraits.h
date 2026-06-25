//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <type_traits>
#include <Amuse/Core/Template/Container/Array.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <Amuse/Core/Template/Container/List.h>
#include <Amuse/Core/Template/Container/Map.h>
#include <Amuse/Core/Template/Container/Set.h>

namespace Amuse {

	//! @ingroup AmuseCore
	template < typename T >
	struct is_sequence : std::false_type {};

	template<typename T, typename ALLOCATOR>
	struct is_sequence<List<T, ALLOCATOR>> : std::true_type {};

	//! @ingroup AmuseCore
	template<typename T, typename ALLOCATOR>
	struct is_sequence<Set<T, ALLOCATOR>> : std::true_type {};

	template<typename T, typename ALLOCATOR>
	struct is_sequence<HashSet<T, ALLOCATOR>> : std::true_type {};

	//! @ingroup AmuseCore
	template<typename T, typename ALLOCATOR>
	struct is_sequence<Vector<T, ALLOCATOR>> : std::true_type {};

	template<typename T, size_t N>
	struct is_sequence<Array<T, N>> : std::true_type {};

	//! @ingroup AmuseCore
	template<typename T, size_t N>
	struct is_sequence<T[N]> : std::true_type {};


	template < typename T >
	struct is_map : std::false_type {};

	//! @ingroup AmuseCore
	template<typename TKey, class TValue>
	struct is_map<Map<TKey, TValue>> : std::true_type {};

	template<typename TKey, class TValue>
	struct is_map<HashMap<TKey, TValue>> : std::true_type {};

}
