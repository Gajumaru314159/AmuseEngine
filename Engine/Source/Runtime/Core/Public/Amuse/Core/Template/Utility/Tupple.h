//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <any>

namespace Amuse {

	template<typename... Types>
	using Tuple = std::tuple<Types...>;

}