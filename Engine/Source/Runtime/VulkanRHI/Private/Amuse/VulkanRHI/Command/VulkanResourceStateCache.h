//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/Barrier.h>

namespace Amuse {
    //! @brief  リソースステート・キャッシュ
    //! @ingroup AmuseVulkanRHI
    class VulkanResourceStateCache {
    public:

        //! @brief  コンストラクタ
        VulkanResourceStateCache() {

        }

        //! @brief  テクスチャの遷移追加
        void addTexture(vk::Image image, vk::ImageLayout before, vk::ImageLayout after, vk::ImageAspectFlags aspectMask) {
            TextureSubresourceRange range;
            addTexture(image, before, after, aspectMask, {}, {}, range);
        }

        //! @brief  テクスチャの遷移追加
        void addTexture(vk::Image image, vk::ImageLayout before, vk::ImageLayout after, vk::ImageAspectFlags aspectMask, vk::AccessFlags beforeAccess, vk::AccessFlags afterAccess, const TextureSubresourceRange& range) {

            auto& barrier = m_barriers.emplace_back();
            barrier = vk::ImageMemoryBarrier();
            barrier.srcAccessMask = beforeAccess;
            barrier.dstAccessMask = afterAccess;
            barrier.oldLayout = before;
            barrier.newLayout = after;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = aspectMask;
            barrier.subresourceRange.baseMipLevel = range.baseMipLevel;
            barrier.subresourceRange.levelCount = range.mipLevelCount == TextureSubresourceRange::All ? VK_REMAINING_MIP_LEVELS : range.mipLevelCount;
            barrier.subresourceRange.baseArrayLayer = range.baseArrayLayer;
            barrier.subresourceRange.layerCount = range.arrayLayerCount == TextureSubresourceRange::All ? VK_REMAINING_ARRAY_LAYERS : range.arrayLayerCount;

        }

        //! @brief  バッファの遷移追加
        void addBuffer(vk::Buffer buffer, vk::AccessFlags beforeAccess, vk::AccessFlags afterAccess) {
            auto& barrier = m_bufferBarriers.emplace_back();
            barrier = vk::BufferMemoryBarrier();
            barrier.srcAccessMask = beforeAccess;
            barrier.dstAccessMask = afterAccess;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer = buffer;
            barrier.offset = 0;
            barrier.size = VK_WHOLE_SIZE;
        }

        //! @brief  UAV相当のメモリバリアを追加
        void addUAV() {
            auto& barrier = m_memoryBarriers.emplace_back();
            barrier = vk::MemoryBarrier();
            barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
            barrier.dstAccessMask = vk::AccessFlags(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
        }

        //! @brief  コマンドを記録
        void recordCommand(vk::CommandBuffer commandBuffer) {

            if (m_barriers.empty() && m_bufferBarriers.empty() && m_memoryBarriers.empty()) {
                return;
            }

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
                {},
                m_memoryBarriers, m_bufferBarriers, m_barriers
            );

            clear();

        }

        //! @brief  追加済みのバリアをコマンドリストに記録
        void clear() {
            m_barriers.clear();
            m_bufferBarriers.clear();
            m_memoryBarriers.clear();
        }

    private:

        Vector<vk::ImageMemoryBarrier> m_barriers;
        Vector<vk::BufferMemoryBarrier> m_bufferBarriers;
        Vector<vk::MemoryBarrier> m_memoryBarriers;

    };

}
