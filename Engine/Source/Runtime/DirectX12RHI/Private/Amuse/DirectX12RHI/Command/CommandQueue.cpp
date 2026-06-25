//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/DirectX12RHI/Command/CommandQueue.h>
#include <Amuse/DirectX12RHI/Command/DirectX12CommandList.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>
#include <Amuse/DirectX12RHI/Utility/Utility.h>

namespace Amuse {
	//! @brief  コンストラクタ
	CommandQueue::CommandQueue(DirectX12Device& device)
		:m_fenceVal(0)
	{
		HRESULT result;

		// コマンドキュー生成
		{
			D3D12_COMMAND_QUEUE_DESC desc{};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			result = device.getNative()->CreateCommandQueue(&desc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf()));
			if (FAILED(result)) {
				Utility::OutputFatalLog(result, "ID3D12Device::CreateCommandQueue()");
				return;
			}
		}
		// フェンス生成
		{
			result = device.getNative()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));
			if (FAILED(result)) {
				Utility::OutputFatalLog(result, "ID3D12Device::CreateFence()");
				return;
			}
		}

		Utility::SetName(m_commandQueue.Get(), "System Command Queue");
		Utility::SetName(m_fence.Get(), "System Fence");

	}

	void CommandQueue::entryCommandList(const CommandList& commandList) {
		m_entriedCommandList.push_back(&commandList);
	}
	void CommandQueue::entryCommandListTop(const CommandList& commandList) {
		m_entriedCommandList.insert(m_entriedCommandList.begin(), &commandList);
	}

	u64 CommandQueue::execute() {
		m_entriedNativeCommandList.clear();
		for (auto& cmdList : m_entriedCommandList) {
			m_entriedNativeCommandList.push_back(static_cast<const DirectX12CommandList*>(cmdList)->getNative());
		}
		m_entriedCommandList.clear();
		if (!m_entriedNativeCommandList.empty()) {
			m_commandQueue->ExecuteCommandLists(static_cast<UINT>(m_entriedNativeCommandList.size()), m_entriedNativeCommandList.data());
		}
		m_commandQueue->Signal(m_fence.Get(), ++m_fenceVal);
		return m_fenceVal;
	}

	void CommandQueue::wait() {
		wait(m_fenceVal);
	}

	void CommandQueue::wait(u64 value) {
		if (m_fence->GetCompletedValue() < value)
		{
			if (auto event = CreateEvent(nullptr, false, false, nullptr)) {
				m_fence->SetEventOnCompletion(value, event);
				WaitForSingleObject(event, INFINITE);
				CloseHandle(event);
			}
		}
	}

	u64 CommandQueue::getCompletedValue() const {
		return m_fence->GetCompletedValue();
	}

	void CommandQueue::queueWait(u64 value) {
		m_commandQueue->Wait(m_fence.Get(), value);
	}

	void CommandQueue::setName(StringView name) {
		Utility::SetName(m_commandQueue.Get(), name);

		String fenceName = Format("{}_Fence", name);
		Utility::SetName(m_fence.Get(), fenceName);
	}

}
