//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      トポロジー
	enum class Topology :u32 {
		PointList,		//!< 頂点リスト
		LineList,		//!< 線分リスト
		TriangleList,	//!< 三角形リスト
	};

}