//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RPI/FrameGraph/FGTexture.h>
#include <Amuse/RPI/FrameGraph/FGBuffer.h>

namespace Amuse::RPI {

	using namespace RHI;

	class FGResourcePool {
	public:

		FGResourcePool(::Amuse::RHI::Device& rhi);

		void update();

		auto createTexture(const FGTexture::Desc& desc) -> Ref<RenderTexture>;
		auto createBuffer(const FGBuffer::Desc& desc) -> Ref<Buffer>;

		void destroyTexture(const FGTexture::Desc& desc, const Ref<RHI::RenderTexture>& texture);
		void destroyBuffer(const FGBuffer::Desc& desc, const Ref<RHI::Buffer>& buffer);

	private:

		template <typename T> struct ResourceEntry { Ref<T> resource; s32 life; };
		template <typename T> using ResourcePool = Vector<ResourceEntry<T>>;

	private:
		RHI::Device& m_rhi;

		HashMap<size_t, ResourcePool<RenderTexture>>	m_texturePools;
		HashMap<size_t, ResourcePool<Buffer>>			m_bufferPools;
	};

}