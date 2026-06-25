//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/VulkanRHI/Descriptor/VulkanDescriptorHandle.h>

namespace Amuse {
	class VulkanDevice;

	//! @brief      バッファ
	//! @ingroup AmuseVulkanRHI
	class VulkanBuffer:public Amuse::Buffer {
	public:

		//! @brief  コンストラクタ
		VulkanBuffer(VulkanDevice& device,const BufferDesc& desc);


		//! @brief  コンストラクタ
		VulkanBuffer(VulkanDevice& device, const BufferDesc& desc, const Blob& blob);


		//! @brief  コンストラクタ
		VulkanBuffer(VulkanDevice& device, const BufferViewDesc& desc);


		//! @brief      名前を取得
		const String& getName()const override;


		//! @brief  定義を取得
		const BufferDesc& getDesc()const override;


		//! @brief      BindlessHandleを取得
		BindlessHandle getHandle()const override;


		//! @brief      バッファを更新
		//! 
		//! @details    map / unmap と異なり、バッファの更新は描画スレッドの直前にまとめて行われます。
		void update(size_t size, const void* pData, size_t offset) override;


		//! @brief      バッファを更新
		//! 
		//! @details    map / unmap と異なり、バッファの更新は描画スレッドの直前にまとめて行われます。
		void update(const CopyFunc& func) override;


		//! @brief      バッファを更新(直接更新)
		void updateDirect(size_t size, const void* pData, size_t offset) override;


		//! @brief      バッファを更新(直接更新)
		void updateDirect(const CopyFunc& func) override;

		//! @brief      CPUからバッファをマップ
		void* map(CpuAccess access) override;

		//! @brief      CPUからのバッファマップを解除
		void unmap() override;

		//! @brief      drawIndirect() 用の引数を一括で書き込む
		void writeDrawParams(Span<const DrawParam> params, u64 offset = 0) override;

		//! @brief      drawIndexedIndirect() 用の引数を一括で書き込む
		void writeDrawIndexedParams(Span<const DrawIndexedParam> params, u64 offset = 0) override;

		//! @brief      dispatchIndirect() 用の引数を一括で書き込む
		void writeDispatchParams(Span<const DispatchParam> params, u64 offset = 0) override;

	public:

		//! @brief      バッファを取得
		vk::Buffer getNative()const { return *m_shared->buffer; }

		//! @brief      ビュー定義を取得
		const BufferViewDesc& getViewDesc()const { return m_viewDesc; }

		//! @brief      TexelBufferViewを取得
		const vk::raii::BufferView& getTexelView()const { return m_texelView; }


	private:

		VulkanDevice& m_device;
		BufferDesc m_desc;
		BufferViewDesc m_viewDesc;
		CpuAccess m_mapAccess = CpuAccess::Read;

		//! @ingroup AmuseVulkanRHI
		struct SharedResource {
			vk::raii::Buffer buffer = nullptr;
			vk::raii::DeviceMemory memory = nullptr;
		};

		SPtr<SharedResource> m_shared;
		VulkanDescriptorHandle m_handle;
		vk::raii::BufferView m_texelView = nullptr;
	};



	//! @cond

	//! @brief      BindlessHandleを取得
	inline BindlessHandle VulkanBuffer::getHandle()const {
		BindlessHandle handle;
		if (m_handle.empty()) return handle;
		handle.type = BindingType::ByteAddressBuffer;
		handle.index = m_handle.getBindlessIndex();
		return handle;
	}


	//! @endcond


}
