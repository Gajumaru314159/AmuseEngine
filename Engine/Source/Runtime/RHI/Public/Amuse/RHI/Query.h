//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>

namespace Amuse {
	//! @brief GPUイベントクエリ
	//! @ingroup AmuseRHI
	class EventQuery : public GraphicObject {
	public:
		//! @brief 生成
		static Ref<EventQuery> Create();
	};

	//! @brief GPUタイマークエリ
	//! @ingroup AmuseRHI
	class TimerQuery : public GraphicObject {
	public:
		//! @brief 生成
		static Ref<TimerQuery> Create();
	};

}

