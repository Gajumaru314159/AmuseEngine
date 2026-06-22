//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/VulkanRHI/Command/VulkanCommandQueue.h>
#include <Amuse/VulkanRHI/Command/VulkanCommandList.h>
#include <Amuse/VulkanRHI/VulkanDevice.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  コンストラクタ
	VulkanCommandQueue::VulkanCommandQueue(VulkanDevice& device) 
		: m_device(device)
	{
		auto& vkdevice = device.getDevice();
		m_queue = vkdevice.getQueue(device.getQueryFamilyIndex(), 0);
		m_device.setName(m_queue, "VulkanCommandQueue");
	}

	//! @brief コマンドリストを登録 
	void VulkanCommandQueue::entryCommandList(const Ref<CommandList>& commandList) {
		m_enteredCommandListVector.push_back(commandList);
	}

	//! @brief コマンドリストを先頭に登録
	void VulkanCommandQueue::entryCommandListTop(const Ref<CommandList>& commandList) {
		m_enteredCommandListVector.insert(m_enteredCommandListVector.begin(), commandList);
	}

	//! @brief 登録したコマンドリストを実行
	u64 VulkanCommandQueue::execute() {

		m_commandBuffers.clear();
		for (auto& commandList : m_enteredCommandListVector) {
			if (auto native = commandList.cast<VulkanCommandList>()) {
				m_commandBuffers.push_back(native->getNative());
			}
		}
		m_enteredCommandListVector.clear();

		vk::SubmitInfo submitInfo;
		submitInfo.setCommandBuffers(m_commandBuffers);

		if (m_commandBuffers.empty()) {
			return m_lastSubmissionValue;
		}

		const u64 fenceValue = ++m_lastSubmissionValue;
		vk::FenceCreateInfo fenceInfo;
		auto fence = m_device.getDevice().createFence(fenceInfo, m_device.getAllocationCallbacks());
		m_queue.submit(submitInfo, *fence);
		m_submissionFences.push_back(SubmissionFence{ fenceValue, std::move(fence) });

		return fenceValue;
	}

	//! @brief 待機
	void VulkanCommandQueue::wait() const {
		wait(m_lastSubmissionValue);
	}

	//! @brief 指定した実行値まで待機
	void VulkanCommandQueue::wait(u64 value) const {
		auto& device = m_device.getDevice();
		for (auto& submission : m_submissionFences) {
			if (submission.value <= value && *submission.fence) {
				[[maybe_unused]] auto result = device.waitForFences({ *submission.fence }, true, UINT64_MAX);
			}
		}
		m_submissionFences.erase(
			std::remove_if(
				m_submissionFences.begin(),
				m_submissionFences.end(),
				[value](const SubmissionFence& submission) { return submission.value <= value; }),
			m_submissionFences.end());
	}

	bool VulkanCommandQueue::isCompleted(u64 value) const {
		if (value == 0) return true;
		if (value > m_lastSubmissionValue) return false;

		for (auto& submission : m_submissionFences) {
			if (submission.value == value && *submission.fence) {
				return submission.fence.getStatus() == vk::Result::eSuccess;
			}
		}
		return true;
	}

}
