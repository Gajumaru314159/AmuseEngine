//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/Button.h>
#include <Amuse/Input/Axis.h>

namespace Amuse::Input {
    using namespace Amuse::Core;

	//! @brief DirectInput デバイス
	class DirectInput {
	public:
		//! @brief 入力デバイスIDを宣言する
		AMUSE_DECL_INPUT_DEVICE_ID("DirectInput");

	public:

	};

}
