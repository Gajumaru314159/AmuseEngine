//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/RootSignature.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  ルートシグネチャ実装(Vulkan)
    class VulkanRootSignature :public Amuse::RHI::RootSignature {
    public:

        //! @brief  コンストラクタ
        VulkanRootSignature(VulkanDevice& device,const RootSignatureDesc& desc);


        //! @brief  デストラクタ
        ~VulkanRootSignature();

        
        //! @brief      名前を取得
        const String& getName()const override;


        //! @brief  定義を取得
        const RootSignatureDesc& getDesc()const noexcept override;

    public:

		vk::PipelineLayout getNative() const noexcept { return *m_pipelineLayout; }

    private:

        RootSignatureDesc m_desc;
		vk::raii::PipelineLayout m_pipelineLayout = nullptr;

    };

}