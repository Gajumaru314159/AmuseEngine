//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/DirectX12RHI/Descriptor/DescriptorHeapType.h>

namespace Amuse::Core {
    struct TLSFBlock;
}

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  説明
    class DescriptorHandle:private Noncopyable {
        friend class DescriptorHeap;
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        DescriptorHandle();
        ~DescriptorHandle();

        DescriptorHandle(DescriptorHandle&&)noexcept;
        DescriptorHandle& operator = (DescriptorHandle&&)noexcept;

        operator bool()const noexcept;

        bool empty()const noexcept;
        void release();
        DescriptorHeapType getHeapType()const;

        D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(s32 index = 0)const;
        D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(s32 index = 0)const;

        u32 getBindlessIndex(s32 index = 0)const;

    private:

        const TLSFBlock* m_pBlock;

    };

}