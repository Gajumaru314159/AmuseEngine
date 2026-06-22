//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Query.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	class VulkanDevice;

	//! @brief GPUイベントクエリ実装(Vulkan)
	class VulkanEventQuery : public EventQuery {
	public:
		//! @brief コンストラクタ
		explicit VulkanEventQuery(VulkanDevice& device);

		//! @brief 妥当な状態か
		bool isValid() const;

		//! @brief 名前を取得
		const String& getName() const override;

		//! @brief 完了判定に使うキューのfence valueを設定
		void signal(u64 fenceValue);

		//! @brief 完了判定に使うキューのfence valueを取得
		u64 getFenceValue() const { return m_fenceValue; }

	private:
		VulkanDevice& m_device;
		String m_name = "VulkanEventQuery";
		u64 m_fenceValue = 0;
	};

	//! @brief GPUタイマークエリ実装(Vulkan)
	class VulkanTimerQuery : public TimerQuery {
	public:
		//! @brief コンストラクタ
		explicit VulkanTimerQuery(VulkanDevice& device);

		//! @brief デストラクタ
		~VulkanTimerQuery() override;

		//! @brief 妥当な状態か
		bool isValid() const;

		//! @brief 名前を取得
		const String& getName() const override;

		//! @brief 開始タイムスタンプを記録
		void recordBegin(vk::CommandBuffer commandBuffer);

		//! @brief 終了タイムスタンプを記録
		void recordEnd(vk::CommandBuffer commandBuffer);

		//! @brief 結果が取得可能か
		bool isResultAvailable() const;

		//! @brief 計測時間を秒単位で取得
		f32 getTime() const;

	private:
		VulkanDevice& m_device;
		String m_name = "VulkanTimerQuery";
		vk::raii::QueryPool m_queryPool = nullptr;
	};

}
