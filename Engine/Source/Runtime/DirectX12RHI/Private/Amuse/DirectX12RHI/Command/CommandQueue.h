//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/CommandList.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  コマンドキュー
    class CommandQueue {
    public:

        //! @brief  コンストラクタ
        CommandQueue(class DirectX12Device& device);

        void entryCommandList(const CommandList& commandList);
        void entryCommandListTop(const CommandList& commandList);

        u64 execute();
        void wait();
        void wait(u64 value);
        u64 getCompletedValue() const;
        void queueWait(u64 value);

        ComPtr<ID3D12CommandQueue>& getNative() {
            return m_commandQueue;
        }

        void setName(StringView name);

    private:

        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceVal;
        HANDLE m_fenceEvent = nullptr;

        Vector<ID3D12CommandList*> m_entriedNativeCommandList;
        Vector<const CommandList*> m_entriedCommandList;

    };

}// Amuse::RHI 
