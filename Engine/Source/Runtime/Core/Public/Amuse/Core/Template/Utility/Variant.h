//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <variant>

namespace Amuse {

	//! @brief バリアント
	template <class... Types>
	using Variant = std::variant<Types...>;

}