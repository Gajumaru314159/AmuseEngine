//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Query.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief 生成
	Ref<EventQuery> EventQuery::Create() {
		if (auto device = Device::Get()) {
			return device->createEventQuery();
		}
		return nullptr;
	}

	//! @brief 生成
	Ref<TimerQuery> TimerQuery::Create() {
		if (auto device = Device::Get()) {
			return device->createTimerQuery();
		}
		return nullptr;
	}

}

