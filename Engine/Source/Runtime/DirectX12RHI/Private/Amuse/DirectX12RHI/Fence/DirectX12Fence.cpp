//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/DirectX12RHI/Fence/DirectX12Fence.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>
#include <Amuse/DirectX12RHI/Utility/Utility.h>

namespace Amuse {
	//! @brief  コンストラクタ
	DirectX12Fence::DirectX12Fence(DirectX12Device& device, const FenceDesc& desc)
		: m_name(desc.name)
	{
		auto result = device.getNative()->CreateFence(desc.initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputFatalLog(result, "ID3D12Device::CreateFence()");
			return;
		}

		m_event = CreateEvent(nullptr, false, false, nullptr);
		Utility::SetName(m_fence.Get(), m_name);
		manage();
	}

	//! @brief  デストラクタ
	DirectX12Fence::~DirectX12Fence() {
		if (m_event) {
			CloseHandle(m_event);
			m_event = nullptr;
		}
	}

	//! @brief  妥当な状態か
	bool DirectX12Fence::isValid() const {
		return m_fence && m_event;
	}

	//! @brief      名前を取得
	const String& DirectX12Fence::getName() const {
		return m_name;
	}

	//! @brief  GPUで完了している値を取得
	u64 DirectX12Fence::getCompletedValue() const {
		return m_fence ? m_fence->GetCompletedValue() : 0;
	}

	//! @brief  指定値までCPUで待機
	void DirectX12Fence::wait(u64 value) {
		if (!m_fence || !m_event) return;
		if (m_fence->GetCompletedValue() >= value) return;
		m_fence->SetEventOnCompletion(value, m_event);
		WaitForSingleObject(m_event, INFINITE);
	}

}
