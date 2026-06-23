//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Render/RenderView.h>
#include <Amuse/Core/Allocator/Utility/SpanAllocator.h>
#include "MaterialRenderFeature.h"
#include "Amuse/Core/Geometry/Bounding.h"
#include "Amuse/RPI/Material/Material.h"
#include "Amuse/RPI/Mesh/Mesh.h"
#include "Amuse/RHI/Buffer.h"
#include "Amuse/RHI/CommandList.h"

namespace Amuse::RPI {

	template<class T>
	class SparseUploadBuffer {
	public:
		class Handle {
		public:
			Span<T> view() {
				return { m_cpu.begin() + m_start, m_cpu.begin() + m_start + m_count };
			}
			T& operator[](size_t index) {
				return m_cpu[index];
			}
			const T& operator[](size_t index) const{
				return m_cpu[index];
			}
		private:
			Vector<T>& m_cpu;
			s32 m_start;
			s32 m_count;
		};
	public:
		Handle alloc(s32 size) {
			Handle handle{ m_cpu, m_allocator.alloc(size), size };
			if (m_cpu.size() < m_allocator.maxSize()) m_cpu.resize(m_allocator.maxSize());
			return handle;
		}
		void free(Handle& handle) {
			m_allocator.free(handle.m_start, handle.m_count);
			handle.m_start = 0;
			handle.m_count = 0;
		}
		void reserve() {
			m_cpu.resize(m_allocator.maxSize());
			if (!m_gpu || m_gpu->getDesc().size < m_cpu.size() * sizeof(T)) {
				m_gpu = RHI::Buffer::Create(RHI::BufferDesc::Structured<T>(m_cpu.size()));
			}
		}
	private:
		Vector<T>			m_cpu;
		Ref<RHI::Buffer>	m_gpu;
		SpanAllocator		m_allocator;
	};

	struct MeshUnitKey
	{
		Ref<Mesh> mesh;
		Vector<Ref<Material>> overrideMaterials;

		bool operator==(const MeshUnitKey& rhs) const
		{
			if (mesh != rhs.mesh) return false;
			return overrideMaterials == rhs.overrideMaterials;
		}
	};
	struct MeshUnitKeyHasher
	{
		size_t operator()(const MeshUnitKey& key) const
		{
			size_t hash = 0;
			Hash::Combine(hash,reinterpret_cast<size_t>(key.mesh.get()));
			for (const auto& material : key.overrideMaterials)
			{
				Hash::Combine(hash, reinterpret_cast<size_t>(material.get()));
			}
			return hash;
		}
	};


	struct MeshProxy
	{
		MeshUnitKey key;
		s32 gindex;
		s32 uindex;
	};

	//! @brief      マテリアル描画機能
	class MeshRenderFeature : public RenderFeature
	{
	public:
		struct UpdateContext {
			RenderScene* scene;
			Span<RenderView*> views;
		};

		enum class MeshCommandType {
			Add,
			Remove,
			Update
		};
		struct MeshCommand
		{
			MeshCommandType type;
			MeshProxy* proxy;
			Matrix matrix;
			Bounds bounds;
		};

	public:

		AMUSE_RTTI();

		explicit MeshRenderFeature(RenderScene& scene) {

		}

		void update(UpdateContext& context);

		MeshProxy* addMesh(const Ref<Mesh>& mesh,const Matrix& matrix)
		{
			auto& command = m_commands.emplace_back();
			command.type = MeshCommandType::Add;
			command.matrix = matrix;
			command.proxy = new MeshProxy();
			command.proxy->key.mesh = mesh;
			command.proxy->key.overrideMaterials;
		}
	private:
		void processCommands(UpdateContext& context);
		void buildPackets(UpdateContext& context);
	private:
		struct MeshUnit {
			Vector<MeshProxy*> proxies;
		};
		struct MeshViewData {
			struct CPUData {
				struct Unit {
					struct LOD {
						Vector<s32> instances;
						s32 instanceStart;
						s32 instanceCount;
					};
					Vector<LOD> lods;
				};
				HashMap<MeshUnitKey,Unit,MeshUnitKeyHasher> units;
				Vector<bool> culls;
				Vector<s8>   lods;
				Vector<s32>  instances;
				Vector<MaterialDrawPacket> packets;
			} cpu;

			struct GPUData {
				Ref<RHI::Buffer> instances;

				void update(const CPUData& cpu);
			} gpu;
		};
	private:
		Vector<MeshCommand> m_commands;

		// パック済み配列
		Vector<MeshProxy*> m_proxies;
		Vector<Matrix> m_matrices;
		Vector<Bounds> m_bounds;

		Ref<RHI::Buffer> m_matrixBuffer;

		SparseUploadBuffer<u32> m_instances;

		HashMap<MeshUnitKey,MeshUnit,MeshUnitKeyHasher> m_units;
	};

}
