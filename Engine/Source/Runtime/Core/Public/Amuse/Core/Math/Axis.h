//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

	//! @brief      軸タイプ
	//! @ingroup AmuseCore
	enum class AxisType :u32 {
		None = 0,
		X = 1,
		Y = 2,
		Z = 4,

		Screen = 8,

		XY = X | Y,
		XZ = X | Z,
		YZ = Y | Z,
		XYZ = X | Y | Z,
	};


}