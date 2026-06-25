//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/ComputePipelineState.h>
#include <Amuse/RHI/Types/ComputePipelineStateDesc.h>
#include <Amuse/VulkanRHI/RootSignature/VulkanRootSignature.h>

//===============================================================
// 前方宣言
//===============================================================
namespace Amuse {
    class VulkanDevice;
}


//===============================================================
// クラス定義
//===============================================================
namespace Amuse {
    //! @brief		コンピュート・パイプライン実装(Vulkan)
    //! @ingroup AmuseVulkanRHI
    class VulkanComputePipelineState :public ComputePipelineState {
    public:

        //! @brief		コンストラクタ
        VulkanComputePipelineState(VulkanDevice&, const ComputePipelineStateDesc& desc);

        //! @brief		デストラクタ
        ~VulkanComputePipelineState() override;

        //! @brief  妥当な状態か
        bool isValid()const { return *m_pipeline; }


        //! @brief      名前を取得
        const String& getName()const override;


        //! @brief      ネイティブオブジェクトを取得
        VkPipeline getNative()const noexcept { return *m_pipeline; }

        VulkanRootSignature* getRootSignature()const { 
            if (auto p = m_desc.rootSignature.cast<VulkanRootSignature>()) {
                return p;
            }
            return nullptr;        
        }

    private:

        const ComputePipelineStateDesc m_desc;
        vk::raii::Pipeline m_pipeline = VK_NULL_HANDLE;
            
    };




    //===============================================================
    // インライン関数
    //===============================================================
    //! @cond

    //! @endcond
}