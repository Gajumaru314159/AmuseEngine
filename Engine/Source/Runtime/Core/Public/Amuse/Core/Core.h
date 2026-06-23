//***********************************************************
//! @file
//! @author Gajumaru
//***********************************************************
#pragma once

#include <Amuse/Core/Exception/Exception.h>

#include <Amuse/Core/File/BinaryReader.h>
#include <Amuse/Core/File/BinaryWriter.h>
#include <Amuse/Core/File/BinaryStream.h>
#include <Amuse/Core/File/File.h>
#include <Amuse/Core/File/Path.h>
#include <Amuse/Core/File/Stream.h>

#include <Amuse/Core/Geometry/Box.h>
#include <Amuse/Core/Geometry/Capsule.h>
#include <Amuse/Core/Geometry/Circle.h>
#include <Amuse/Core/Geometry/Cylinder.h>
#include <Amuse/Core/Geometry/Frustum.h>
#include <Amuse/Core/Geometry/HalfLine.h>
#include <Amuse/Core/Geometry/IntRect.h>
#include <Amuse/Core/Geometry/Line.h>
#include <Amuse/Core/Geometry/Margin.h>
#include <Amuse/Core/Geometry/Plane.h>
#include <Amuse/Core/Geometry/Point.h>
#include <Amuse/Core/Geometry/Rect.h>
#include <Amuse/Core/Geometry/Segment.h>
#include <Amuse/Core/Geometry/Size.h>
#include <Amuse/Core/Geometry/Sphere.h>
#include <Amuse/Core/Geometry/Spline.h>
#include <Amuse/Core/Geometry/Spline2D.h>
#include <Amuse/Core/Geometry/Triangle.h>
#include <Amuse/Core/Geometry/Viewport.h>

#include <Amuse/Core/Graphic/Color.h>
#include <Amuse/Core/Graphic/HSV.h>
#include <Amuse/Core/Graphic/IntColor.h>

#include <Amuse/Core/Log/Assertion.h>
#include <Amuse/Core/Log/LogMacro.h>

#include <Amuse/Core/Math/Axis.h>
#include <Amuse/Core/Math/BitOp.h>
#include <Amuse/Core/Math/Easing.h>
#include <Amuse/Core/Math/FloatCompression.h>
#include <Amuse/Core/Math/Vectors.h>
#include <Amuse/Core/Math/Math.h>
#include <Amuse/Core/Math/Matrix.h>
#include <Amuse/Core/Math/Quaternion.h>
#include <Amuse/Core/Math/Random.h>
#include <Amuse/Core/Math/Range.h>
#include <Amuse/Core/Math/Rotation.h>
#include <Amuse/Core/Math/Transform.h>

#include <Amuse/Core/Memory/MemoryUtility.h>

#include <Amuse/Core/Misc/Blob.h>
#include <Amuse/Core/Misc/BlobView.h>
#include <Amuse/Core/Misc/CAPI.h>
#include <Amuse/Core/Misc/Compression.h>
#include <Amuse/Core/Misc/CRCHash.h>
#include <Amuse/Core/Misc/DateTime.h>
#include <Amuse/Core/Misc/TimeSpan.h>
#include <Amuse/Core/Misc/Noise.h>
#include <Amuse/Core/Misc/Scheduler.h>
#include <Amuse/Core/Misc/UUID.h>
#include <Amuse/Core/Misc/YesNo.h>

#include <Amuse/Core/Reflection/Type.h>

#include <Amuse/Core/String/Format.h>
#include <Amuse/Core/String/String.h>
#include <Amuse/Core/String/StringEncoder.h>

#include <Amuse/Core/Template/include.h>

#include <Amuse/Core/Thread/Atomic.h>
#include <Amuse/Core/Thread/Mutex.h>
#include <Amuse/Core/Thread/ScopeLock.h>
#include <Amuse/Core/Thread/SpinLock.h>
#include <Amuse/Core/Thread/Thread.h>

#include <Amuse/Core/Utility/ConstValue.h>
#include <Amuse/Core/Utility/Indexed.h>
#include <Amuse/Core/Utility/MemoryStorage.h>
#include <Amuse/Core/Utility/Noncopyable.h>
#include <Amuse/Core/Utility/Nonmovable.h>
#include <Amuse/Core/Utility/Pimpl.h>
#include <Amuse/Core/Utility/Scope.h>
#include <Amuse/Core/Utility/Ref.h>
#include <Amuse/Core/Utility/Singleton.h>
#include <Amuse/Core/Utility/Name.h>

#include <Amuse/Core/CoreForward.h>

namespace Amuse {
	using namespace Amuse::Core;
}