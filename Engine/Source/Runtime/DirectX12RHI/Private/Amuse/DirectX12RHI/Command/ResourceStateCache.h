//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/DirectX12RHI/Texture/DirectX12Texture.h>
#include <Amuse/DirectX12RHI/Buffer/DirectX12Buffer.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  リソースステート・キャッシュ
    class ResourceStateCache {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //! @brief  コンストラクタ
        ResourceStateCache() {

        }

        //! @brief  バッファの遷移追加
        void addBuffer(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
            addTransition(resource, before, after);
        }

        //! @brief  テクスチャの遷移追加
        void addTexture(DirectX12Texture& texture, D3D12_RESOURCE_STATES after, s32 subresource = -1) {

            D3D12_RESOURCE_BARRIER barrier{};
            if (texture.addResourceTransition(barrier, after, subresource)) 
            {
                m_barriers.push_back(barrier);
            }

        }

        //! @brief  テクスチャの遷移追加
        void addTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, s32 subresource = -1) {
            addTransition(resource, before, after, subresource);
        }

        //! @brief  UAVバリアを追加
        void addUAV(ID3D12Resource* resource) {
            auto& desc = m_barriers.emplace_back();
            desc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            desc.UAV.pResource = resource;
        }

        //! @brief  コマンドを記録
        void recordCommand(ID3D12GraphicsCommandList& commandList) {

            if (m_barriers.empty()) {
                return;
            }

            commandList.ResourceBarrier(static_cast<UINT>(m_barriers.size()), m_barriers.data());

            m_barriers.clear();

            clear();

        }

        //! @brief  追加済みのバリアをコマンドリストに記録
        void clear() {
            m_barriers.clear();
        }

    private:

        //! @brief  リソースのステート遷移を追加
        void addTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, s32 subresource = -1) {

            if (before == after) {

                if (before == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
                    addUAV(resource);

                }

                return;
            }

            auto& desc = m_barriers.emplace_back();
            desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            desc.Transition.pResource = resource;
            desc.Transition.Subresource = subresource < 0 ? D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES : static_cast<UINT>(subresource);
            desc.Transition.StateBefore = before;
            desc.Transition.StateAfter = after;

        }

        //! @brief  リソースのエイリアシング遷移を追加
        void addTransition(ID3D12Resource* before, ID3D12Resource* after) {

            auto& desc = m_barriers.emplace_back();
            desc.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
            desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            desc.Aliasing.pResourceBefore = before;
            desc.Aliasing.pResourceAfter = after;

        }

    private:

        Vector<D3D12_RESOURCE_BARRIER> m_barriers;

    };

}// Amuse::RHI 
