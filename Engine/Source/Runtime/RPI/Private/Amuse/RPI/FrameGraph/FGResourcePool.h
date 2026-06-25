//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RPI/FrameGraph/FGTexture.h>
#include <Amuse/RPI/FrameGraph/FGBuffer.h>

namespace Amuse {
	//! @ingroup AmuseRPI
	class FGResourcePool {
	public:

		FGResourcePool(::Amuse::Device& rhi);

		void update();

		auto createTexture(const FGTexture::Desc& desc) -> Ref<RenderTexture>;
		auto createBuffer(const FGBuffer::Desc& desc) -> Ref<Buffer>;

		void destroyTexture(const FGTexture::Desc& desc, const Ref<RenderTexture>& texture);
		void destroyBuffer(const FGBuffer::Desc& desc, const Ref<Buffer>& buffer);

	private:

		//! @ingroup AmuseRPI
		template <typename T> struct ResourceEntry { Ref<T> resource; s32 life; };
		template <typename T> using ResourcePool = Vector<ResourceEntry<T>>;

	private:
		Device& m_rhi;

		HashMap<size_t, ResourcePool<RenderTexture>>	m_texturePools;
		HashMap<size_t, ResourcePool<Buffer>>			m_bufferPools;
	};

}