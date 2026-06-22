//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>

namespace Amuse::Core {
    struct TLSFBlock;
}

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  Bindless用にグローバルDescriptorHeapから割り当てられたDescriptorにアクセスするためのハンドル
    class VulkanDescriptorHandle:private Noncopyable {
        friend class VulkanDescriptorHeap;
    public:

        VulkanDescriptorHandle();
        ~VulkanDescriptorHandle();

        VulkanDescriptorHandle(VulkanDescriptorHandle&&)noexcept;
        VulkanDescriptorHandle& operator = (VulkanDescriptorHandle&&)noexcept;

        operator bool()const noexcept;

        bool empty()const noexcept;
        void release();

        u32 getBindlessIndex(s32 index = 0)const;

    private:

        const TLSFBlock* m_pBlock;

    };

}