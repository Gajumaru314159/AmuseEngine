//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/VulkanRHI/Query/VulkanTimerQuery.h>
#include <Amuse/VulkanRHI/VulkanDevice.h>

namespace Amuse {
	//! @brief コンストラクタ
	VulkanEventQuery::VulkanEventQuery(VulkanDevice& device)
		: m_device(device)
	{
	}

	//! @brief 妥当な状態か
	bool VulkanEventQuery::isValid() const {
		return true;
	}

	//! @brief 名前を取得
	const String& VulkanEventQuery::getName() const {
		return m_name;
	}

	//! @brief 完了判定に使うキューのfence valueを設定
	void VulkanEventQuery::signal(u64 fenceValue) {
		m_fenceValue = fenceValue;
	}

	//! @brief コンストラクタ
	VulkanTimerQuery::VulkanTimerQuery(VulkanDevice& device)
		: m_device(device)
	{
		vk::QueryPoolCreateInfo info;
		info.queryType = vk::QueryType::eTimestamp;
		info.queryCount = 2;

		m_queryPool = m_device.getDevice().createQueryPool(info, m_device.getAllocationCallbacks());
		m_device.setName(m_queryPool, m_name);

		manage();
	}

	//! @brief デストラクタ
	VulkanTimerQuery::~VulkanTimerQuery() = default;

	//! @brief 妥当な状態か
	bool VulkanTimerQuery::isValid() const {
		return *m_queryPool;
	}

	//! @brief 名前を取得
	const String& VulkanTimerQuery::getName() const {
		return m_name;
	}

	//! @brief 開始タイムスタンプを記録
	void VulkanTimerQuery::recordBegin(vk::CommandBuffer commandBuffer) {
		commandBuffer.resetQueryPool(*m_queryPool, 0, 2);
		commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *m_queryPool, 0);
	}

	//! @brief 終了タイムスタンプを記録
	void VulkanTimerQuery::recordEnd(vk::CommandBuffer commandBuffer) {
		commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *m_queryPool, 1);
	}

	//! @brief 結果が取得可能か
	bool VulkanTimerQuery::isResultAvailable() const {
		u64 timestamps[2]{};
		auto result = static_cast<VkResult>(vkGetQueryPoolResults(
			*m_device.getDevice(),
			*m_queryPool,
			0,
			2,
			sizeof(timestamps),
			timestamps,
			sizeof(u64),
			VK_QUERY_RESULT_64_BIT));
		return result == VK_SUCCESS;
	}

	//! @brief 計測時間を秒単位で取得
	f32 VulkanTimerQuery::getTime() const {
		u64 timestamps[2]{};
		auto result = static_cast<VkResult>(vkGetQueryPoolResults(
			*m_device.getDevice(),
			*m_queryPool,
			0,
			2,
			sizeof(timestamps),
			timestamps,
			sizeof(u64),
			VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
		if (result != VK_SUCCESS || timestamps[1] < timestamps[0]) return 0.0f;

		const double nanoseconds = static_cast<double>(timestamps[1] - timestamps[0]) * static_cast<double>(m_device.getLimits().timestampPeriod);
		return static_cast<f32>(nanoseconds * 1.0e-9);
	}

}
