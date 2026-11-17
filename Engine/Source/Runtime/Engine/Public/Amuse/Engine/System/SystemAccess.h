//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
	
	struct SystemAccess {
		Vector<Type> writes;
		Vector<Type> reads;

		template<class... T>
		SystemAccess& write() {
			(writes.emplace_back(Type::Get<T>()), ...);
			return *this;
		}

		template<class... T>
		SystemAccess& read() {
			(reads.emplace_back(Type::Get<T>()), ...);
			return *this;
		}
	};

}