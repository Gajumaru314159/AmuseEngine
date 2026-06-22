//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/PipelineState.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief		グラフィック・パイプライン実装(DirectX12)
    class VulkanPipelineState :public PipelineState {
    public:

        //! @brief		コンストラクタ
        VulkanPipelineState(VulkanDevice& device, const PipelineStateDesc& desc);

        //! @brief      定義を取得
        const PipelineStateDesc& getDesc()const { return m_desc; };

    public:

        //! @brief      パイプラインを取得
		vk::raii::Pipeline& getNative() { return m_pipeline; }

		vk::PipelineLayout getLayout() const { return m_layout; }

    private:

        const String& getName() const override;

    private:

        PipelineStateDesc       m_desc;
	    vk::raii::Pipeline      m_pipeline = nullptr;
		vk::PipelineLayout      m_layout = nullptr;

    };

}