//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief GPUイベントクエリ
	class EventQuery : public GraphicObject {
	public:
		//! @brief 生成
		static Ref<EventQuery> Create();
	};

	//! @brief GPUタイマークエリ
	class TimerQuery : public GraphicObject {
	public:
		//! @brief 生成
		static Ref<TimerQuery> Create();
	};

}

