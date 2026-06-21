//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <optional>

namespace Amuse::Core {

	//! @brief オプショナル
	template <class T>
	using Optional = std::optional<T>;

}