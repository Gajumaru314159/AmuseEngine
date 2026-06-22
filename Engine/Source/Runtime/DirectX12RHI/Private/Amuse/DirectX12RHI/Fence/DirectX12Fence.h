//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Fence.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	class DirectX12Device;

	//! @brief  DirectX12 GPU同期フェンス
	class DirectX12Fence : public Fence {
	public:

		//! @brief  コンストラクタ
		DirectX12Fence(DirectX12Device& device, const FenceDesc& desc);

		//! @brief  デストラクタ
		~DirectX12Fence() override;

		//! @brief  妥当な状態か
		bool isValid() const;

		//! @brief      名前を取得
		const String& getName() const override;

		//! @brief  GPUで完了している値を取得
		u64 getCompletedValue() const override;

		//! @brief  指定値までCPUで待機
		void wait(u64 value) override;

		//! @brief  ネイティブフェンスを取得
		ID3D12Fence* getNative() const { return m_fence.Get(); }

	private:
		String m_name;
		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_event = nullptr;
	};

}
