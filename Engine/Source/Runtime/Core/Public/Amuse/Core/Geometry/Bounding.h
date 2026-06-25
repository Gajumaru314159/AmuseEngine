//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Math/Quaternion.h>
#include <Amuse/Core/Geometry/Box.h>
#include <Amuse/Core/Geometry/Sphere.h>

namespace Amuse {

	//! @brief		Axis Aligned Bounding Box
	//! @ingroup AmuseCore
	struct AABB {
		Vec3 min; //!< 最小座標
		Vec3 max; //!< 最大座標
	};

	//! @brief		Oriented Bounding Box
	//! @ingroup AmuseCore
	struct OBB {
		Vec3 pos; //!< 中心座標
		Vec3 size; //!< バウンディングサイズ
		Quat rotation; //!< バウンディングの回転量
	};
	
	//! @brief		Box と Sphere
	//! @ingroup AmuseCore
	struct Bounds {
		Box		box; //!< Box Bounds
		Sphere	sphere; //!< Sphere Bounds
	};

}
