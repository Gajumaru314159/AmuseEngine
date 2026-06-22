//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/DirectX12RHI/Query/DirectX12TimerQuery.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>
#include <Amuse/DirectX12RHI/Utility/Utility.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief コンストラクタ
	DirectX12EventQuery::DirectX12EventQuery(DirectX12Device& device)
		: m_device(device)
	{
	}

	//! @brief 妥当な状態か
	bool DirectX12EventQuery::isValid() const {
		return true;
	}

	//! @brief 名前を取得
	const String& DirectX12EventQuery::getName() const {
		return m_name;
	}

	//! @brief 完了判定に使うキューのfence valueを設定
	void DirectX12EventQuery::signal(u64 fenceValue) {
		m_fenceValue = fenceValue;
	}

	//! @brief コンストラクタ
	DirectX12TimerQuery::DirectX12TimerQuery(DirectX12Device& device)
		: m_device(device)
	{
		D3D12_QUERY_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		heapDesc.Count = 2;
		heapDesc.NodeMask = 0;

		auto result = m_device.getNative()->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(m_queryHeap.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputFatalLog(result, "ID3D12Device::CreateQueryHeap()");
			return;
		}

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(u64) * 2);
		result = m_device.getNative()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_readbackBuffer.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputFatalLog(result, "ID3D12Device::CreateCommittedResource()");
			return;
		}

		Utility::SetName(m_queryHeap.Get(), m_name);
		Utility::SetName(m_readbackBuffer.Get(), m_name);

		manage();
	}

	//! @brief 妥当な状態か
	bool DirectX12TimerQuery::isValid() const {
		return m_queryHeap && m_readbackBuffer;
	}

	//! @brief 名前を取得
	const String& DirectX12TimerQuery::getName() const {
		return m_name;
	}

	//! @brief 開始タイムスタンプを記録
	void DirectX12TimerQuery::recordBegin(ID3D12GraphicsCommandList& commandList) {
		commandList.EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
	}

	//! @brief 終了タイムスタンプを記録
	void DirectX12TimerQuery::recordEnd(ID3D12GraphicsCommandList& commandList) {
		commandList.EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
		commandList.ResolveQueryData(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, 2, m_readbackBuffer.Get(), 0);
	}

	//! @brief 計測時間を秒単位で取得
	f32 DirectX12TimerQuery::getTime() const {
		if (!m_readbackBuffer) return 0.0f;

		u64 timestamps[2]{};
		void* mapped = nullptr;
		D3D12_RANGE range{ 0, sizeof(timestamps) };
		if (FAILED(m_readbackBuffer->Map(0, &range, &mapped))) {
			return 0.0f;
		}
		std::memcpy(timestamps, mapped, sizeof(timestamps));
		D3D12_RANGE writeRange{ 0, 0 };
		m_readbackBuffer->Unmap(0, &writeRange);

		u64 frequency = 0;
		m_device.getCommandQueue()->GetTimestampFrequency(&frequency);
		if (frequency == 0 || timestamps[1] < timestamps[0]) return 0.0f;
		return static_cast<f32>(static_cast<double>(timestamps[1] - timestamps[0]) / static_cast<double>(frequency));
	}

}
