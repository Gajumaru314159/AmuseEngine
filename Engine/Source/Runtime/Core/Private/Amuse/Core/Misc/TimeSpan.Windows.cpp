//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Misc/TimeSpan.h>

#ifdef OS_WINDOWS
#include <Windows.h>

namespace Amuse::Core {

	//! @brief  システムを起動してからの時間
	TimeSpan TimeSpan::FromSystemLaunch() {
		return TimeSpan::MilliSeconds(gsl::narrow_cast<f64>(GetTickCount64()));
	}

}

#endif