//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>

namespace Amuse {
	//! @brief Vulkan RHI 設定
	//! @ingroup AmuseVulkanRHI
	struct VulkanRHIConfig {
		bool enableDebugLayer = false; //!< デバッグレイヤー有効フラグ
		LogLevel logLevel = LogLevel::Warning; //!< logLevel
	};

}
