//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief Vulkan RHI 設定
	struct VulkanRHIConfig {
		bool enableDebugLayer = false; //!< デバッグレイヤー有効フラグ
		LogLevel logLevel = LogLevel::Warning; //!< logLevel
	};

}
