//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/VulkanRHI/Fence/VulkanFence.h>
#include <Amuse/VulkanRHI/VulkanDevice.h>

namespace Amuse {
	//! @brief  コンストラクタ
	VulkanFence::VulkanFence(VulkanDevice& device, const FenceDesc& desc)
		: m_device(device)
		, m_name(desc.name)
	{
		vk::SemaphoreTypeCreateInfo typeInfo;
		typeInfo.semaphoreType = vk::SemaphoreType::eTimeline;
		typeInfo.initialValue = desc.initialValue;

		vk::SemaphoreCreateInfo info;
		info.pNext = &typeInfo;

		m_semaphore = m_device.getDevice().createSemaphore(info, m_device.getAllocationCallbacks());
		m_device.setName(m_semaphore, m_name);
		manage();
	}

	//! @brief      名前を取得
	const String& VulkanFence::getName() const {
		return m_name;
	}

	//! @brief  GPUで完了している値を取得
	u64 VulkanFence::getCompletedValue() const {
		u64 value = 0;
		vkGetSemaphoreCounterValue(*m_device.getDevice(), *m_semaphore, &value);
		return value;
	}

	//! @brief  指定値までCPUで待機
	void VulkanFence::wait(u64 value) {
		VkSemaphore semaphore = *m_semaphore;
		VkSemaphoreWaitInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		info.semaphoreCount = 1;
		info.pSemaphores = &semaphore;
		info.pValues = &value;
		vkWaitSemaphores(*m_device.getDevice(), &info, UINT64_MAX);
	}

}
