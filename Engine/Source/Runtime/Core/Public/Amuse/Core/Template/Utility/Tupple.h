//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <any>

namespace Amuse::Core {

	template<typename... Types>
	using Tuple = std::tuple<Types...>;

}