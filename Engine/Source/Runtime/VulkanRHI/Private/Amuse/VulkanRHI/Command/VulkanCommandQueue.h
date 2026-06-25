//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/CommandList.h>

namespace Amuse {
    //! @brief  コマンドキュー
    //! @ingroup AmuseVulkanRHI
    class VulkanCommandQueue {
    public:

        //! @brief  コンストラクタ
        VulkanCommandQueue(VulkanDevice& device);

        void entryCommandList(const Ref<CommandList>& commandList);
        void entryCommandListTop(const Ref<CommandList>& commandList);

        u64 execute();
        void wait() const;
        void wait(u64 value) const;
        bool isCompleted(u64 value) const;

		vk::Queue getQueue() const {
			return *m_queue;
		}

    private:
        VulkanDevice&               m_device;
        vk::raii::Queue             m_queue = nullptr;

        Vector<Ref<CommandList>>    m_enteredCommandListVector;

        Vector<vk::CommandBuffer>   m_commandBuffers;

        //! @ingroup AmuseVulkanRHI
        struct SubmissionFence {
            u64 value = 0;
            vk::raii::Fence fence = nullptr;
        };
        mutable Vector<SubmissionFence> m_submissionFences;
        u64 m_lastSubmissionValue = 0;
    };

}
