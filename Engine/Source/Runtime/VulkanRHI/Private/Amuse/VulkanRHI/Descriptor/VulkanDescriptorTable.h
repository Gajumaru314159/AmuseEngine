//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/DescriptorTable.h>

namespace Amuse {
	class VulkanDescriptorLayout;

    //! @brief  デスクリプタ・テーブル実装(DirectX12)
    //! @ingroup AmuseVulkanRHI
    class VulkanDescriptorTable :public DescriptorTable {
    public:

        //! @brief              コンストラクタ
        VulkanDescriptorTable(VulkanDevice& device, const DescriptorTableDesc& desc);


		//! @brief      名前を取得
		const String& getName()const override;

		const DescriptorTableDesc& getDesc()const override { return m_desc; }


		//! @brief  リソースを設定
		//! @{
		bool setResource(s32 index, const Ref<Buffer>& resource) override;
		bool setResource(s32 index, const Ref<Texture>& resource) override;
		bool setResource(s32 index, const Ref<Sampler>& resource) override;
		//! @}

	public:

		void record(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipeline, s32 slot) const;
		void recordCompute(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipeline, s32 slot) const;

	private:

		bool tryGetRangeType(s32 index, const Ref<Amuse::Buffer>& buffer, vk::DescriptorType& type) const;
		bool tryGetRangeType(s32 index, const Ref<Amuse::Texture>& texture, vk::DescriptorType& type) const;
		bool tryGetRangeType(s32 index, const Ref<Amuse::Sampler>& sampler, vk::DescriptorType& type) const;

	private:

		//! @ingroup AmuseVulkanRHI
		struct BufferElement {
			Ref<Buffer> resource;
		};
		struct TextureElement {
			Ref<Texture> resource;
			vk::raii::ImageView view;
		};
		//! @ingroup AmuseVulkanRHI
		struct SamplerElement {
			Ref<Sampler> resource;
		};

		using Element = Variant<std::monostate, BufferElement, TextureElement, SamplerElement>;

    private:
		VulkanDevice&			m_device;
		DescriptorTableDesc		m_desc;

		vk::raii::DescriptorPool m_pool = nullptr;
		vk::raii::DescriptorSet m_set = nullptr;
		Vector<Element>			m_elements;

		VulkanDescriptorLayout* m_layout = nullptr; // m_desc.layoutのキャッシュ
    };

}
