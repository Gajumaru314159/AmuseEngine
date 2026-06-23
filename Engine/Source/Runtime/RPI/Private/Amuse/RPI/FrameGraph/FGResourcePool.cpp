//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/FrameGraph/FGResourcePool.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RPI/FrameGraph/FGTexture.h>
#include <Amuse/RPI/FrameGraph/FGBuffer.h>

namespace std {
	
	inline void hash_combine(std::size_t& seed) { }

	template <typename T, typename... Rest>
	inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		hash_combine(seed, rest...);
	}

	template<>
	struct hash<::Amuse::RHI::RenderTextureDesc> {
		size_t operator()(const ::Amuse::RHI::RenderTextureDesc& desc) {
			size_t h = 0;
			hash_combine(
				h, 
				desc.format, 
				desc.size.width, 
				desc.size.height, 
				desc.size.depth
			);
			return h;
		}
	};

	template<>
	struct hash<::Amuse::RHI::BufferDesc> {
		size_t operator()(const ::Amuse::RHI::BufferDesc& desc) {
			size_t h = 0;
			hash_combine(
				h, 
				Amuse::Core::enum_cast(desc.state),
				Amuse::Core::enum_cast(desc.state),
				desc.size, 
				desc.stride,
				(Amuse::Core::u32)desc.flags
			);
			return h;
		}
	};

}


namespace Amuse::RPI {

	FGResourcePool::FGResourcePool(::Amuse::RHI::Device& rhi)
		: m_rhi(rhi)
	{
	}

	void FGResourcePool::update() {

		static const s32 s_idleFrame = 10;

		for (auto& [hash, pool] : m_texturePools) {
			for (auto& [texture, life] : pool) {
				life++;
			}
			for (auto itr = pool.begin(); itr != pool.end();) {
				if (s_idleFrame < itr->life) {
					itr = pool.erase(itr);
				} else {
					itr++;
				}
			}
		}

		for (auto& [hash, pool] : m_bufferPools) {
			for (auto& [texture, life] : pool) {
				life++;
			}
			for (auto itr = pool.begin(); itr != pool.end();) {
				if (s_idleFrame < itr->life) {
					itr = pool.erase(itr);
				} else {
					itr++;
				}
			}
		}

	}

	auto FGResourcePool::createTexture(const FGTexture::Desc& desc) ->Ref<::Amuse::RHI::RenderTexture> {
		const auto hash = std::hash<FGTexture::Desc>{}(desc);
		auto& pool = m_texturePools[hash];
		if (pool.empty()) {
			return RenderTexture::Create(desc);
		} else {
			auto resource = pool.back().resource;
			resource->setName(desc.name);
			pool.pop_back();
			return resource;
		}
	}
	auto FGResourcePool::createBuffer(const FGBuffer::Desc& desc) -> Ref<::Amuse::RHI::Buffer> {
		const auto hash = std::hash<FGBuffer::Desc>{}(desc);
		auto& pool = m_bufferPools[hash];
		if (pool.empty()) {
			return Buffer::Create(desc);
		} else {
			auto resource = pool.back().resource;
			pool.pop_back();
			return resource;
		}
	}
	
	void FGResourcePool::destroyTexture(const FGTexture::Desc& desc, const Ref<RHI::RenderTexture>& texture) {
		const auto h = std::hash<FGTexture::Desc>{}(desc);
		m_texturePools[h].push_back({ texture, 0 });
	}
	void FGResourcePool::destroyBuffer(const FGBuffer::Desc& desc, const Ref<RHI::Buffer>& buffer) {
		const auto h = std::hash<FGBuffer::Desc>{}(desc);
		m_bufferPools[h].push_back({ buffer, 0 });
	}

}