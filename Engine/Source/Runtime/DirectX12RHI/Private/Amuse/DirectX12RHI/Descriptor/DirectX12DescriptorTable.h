//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/DescriptorTable.h>
#include <Amuse/RHI/Texture.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/DirectX12RHI/Descriptor/DescriptorHandle.h>
#include <Amuse/DirectX12RHI/RootSignature/DirectX12RootSignature.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;
	class DirectX12Device;
	class DescriptorHeap;
	class DirectX12DescriptorLayout;
}

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  デスクリプタ・テーブル実装(DirectX12)
    class DirectX12DescriptorTable :public DescriptorTable {
    public:

        //! @brief              コンストラクタ
        DirectX12DescriptorTable(DirectX12Device& device, const DescriptorTableDesc& desc, DescriptorHeap& heap, DescriptorHeap& heap2);
		

		//! @brief  妥当な状態か
		bool isValid()const;


		//! @brief      名前を取得
		const String& getName()const override;

		const DescriptorTableDesc& getDesc() const override { return m_desc;  }

		//! @brief  リソースを設定
		bool setResource(s32 index, const Ref<Buffer>& resource) override;
		bool setResource(s32 index, const Ref<Texture>& resource) override;
		bool setResource(s32 index, const Ref<Sampler>& resource) override;

	public:

		void record(ID3D12GraphicsCommandList& cmdList, const DirectX12RootSignature& signature,s32 slot) const;
		void recordCompute(ID3D12GraphicsCommandList& cmdList, const DirectX12RootSignature& signature, s32 slot) const;

	private:

		bool tryGetRangeType(s32 index, const Ref<Amuse::RHI::Buffer>& buffer, D3D12_DESCRIPTOR_RANGE_TYPE& type) const;
		bool tryGetRangeType(s32 index, const Ref<Amuse::RHI::Texture>& texture, D3D12_DESCRIPTOR_RANGE_TYPE& type) const;
		bool tryGetRangeType(s32 index, const Ref<Amuse::RHI::Sampler>& sampler, D3D12_DESCRIPTOR_RANGE_TYPE& type) const;

    private:
		DirectX12Device&		m_device;
		DescriptorTableDesc m_desc;
		DirectX12DescriptorLayout* m_layout = nullptr;

		DescriptorHandle	m_samplerHandle;
		DescriptorHandle	m_othersHandle;

		using Element = Variant<std::monostate,Ref<Buffer>, Ref<Texture>, Ref<Sampler>>;

		Vector<Element> m_elements;

    };

}
