//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Math/Quaternion.h>
#include <Amuse/Core/Geometry/Box.h>
#include <Amuse/Core/Geometry/Sphere.h>

namespace Amuse::Core {

	//! @brief		Axis Aligned Bounding Box
	struct AABB {
		Vec3 min;
		Vec3 max;
	};

	//! @brief		Oriented Bounding Box
	struct OBB {
		Vec3 pos;
		Vec3 size;
		Quat rotation;
	};
	
	//! @brief		Box と Sphere
	struct Bounds {
		Box		box;
		Sphere	sphere;
	};

}