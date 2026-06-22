//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Fence.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	class VulkanDevice;

	//! @brief  Vulkan GPU同期フェンス
	class VulkanFence : public Fence {
	public:

		//! @brief  コンストラクタ
		VulkanFence(VulkanDevice& device, const FenceDesc& desc);

		//! @brief      名前を取得
		const String& getName() const override;

		//! @brief  GPUで完了している値を取得
		u64 getCompletedValue() const override;

		//! @brief  指定値までCPUで待機
		void wait(u64 value) override;

	private:
		VulkanDevice& m_device;
		String m_name;
		vk::raii::Semaphore m_semaphore = nullptr;
	};

}
