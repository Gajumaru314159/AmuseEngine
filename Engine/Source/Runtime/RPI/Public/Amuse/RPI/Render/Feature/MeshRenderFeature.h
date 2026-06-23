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
	//! @brief CPU 側の疎配列領域を GPU バッファへアップロードするバッファ
	class SparseUploadBuffer {
	public:
		//! @brief SparseUploadBuffer の割り当て範囲
		class Handle {
		public:
			//! @brief 割り当て範囲を Span として取得する
			Span<T> view() {
				return { m_cpu.begin() + m_start, m_cpu.begin() + m_start + m_count };
			}
			//! @brief 要素へアクセスする
			T& operator[](size_t index) {
				return m_cpu[index];
			}
			//! @brief 要素へアクセスする
			const T& operator[](size_t index) const{
				return m_cpu[index];
			}
		private:
			Vector<T>& m_cpu;
			s32 m_start;
			s32 m_count;
		};
	public:
		//! @brief 指定要素数の領域を確保する
		Handle alloc(s32 size) {
			Handle handle{ m_cpu, m_allocator.alloc(size), size };
			if (m_cpu.size() < m_allocator.maxSize()) m_cpu.resize(m_allocator.maxSize());
			return handle;
		}
		//! @brief 確保済み領域を解放する
		void free(Handle& handle) {
			m_allocator.free(handle.m_start, handle.m_count);
			handle.m_start = 0;
			handle.m_count = 0;
		}
		//! @brief GPU バッファ容量を CPU 側容量に合わせる
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

	//! @brief 同一描画単位を識別するキー
	struct MeshUnitKey
	{
		Ref<Mesh> mesh; //!< メッシュ
		Vector<Ref<Material>> overrideMaterials; //!< 上書きマテリアル

		//! @brief キーの等価性を判定する
		bool operator==(const MeshUnitKey& rhs) const
		{
			if (mesh != rhs.mesh) return false;
			return overrideMaterials == rhs.overrideMaterials;
		}
	};
	//! @brief MeshUnitKey 用ハッシュ
	struct MeshUnitKeyHasher
	{
		//! @brief MeshUnitKey のハッシュ値を計算する
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


	//! @brief シーン内に配置されたメッシュインスタンス
	struct MeshProxy
	{
		MeshUnitKey key; //!< 描画単位キー
		s32 gindex; //!< グローバルインデックス
		s32 uindex; //!< ユニット内インデックス
	};

	//! @brief      マテリアル描画機能
	class MeshRenderFeature : public RenderFeature
	{
	public:
		//! @brief メッシュ更新時の参照情報
		struct UpdateContext {
			RenderScene* scene; //!< 描画シーン
			Span<RenderView*> views; //!< 描画ビュー
		};

		//! @brief メッシュ更新コマンド種別
		enum class MeshCommandType {
			Add,
			Remove,
			Update
		};
		//! @brief メッシュ更新コマンド
		struct MeshCommand
		{
			MeshCommandType type; //!< コマンド種別
			MeshProxy* proxy; //!< 対象プロキシ
			Matrix matrix; //!< ワールド行列
			Bounds bounds; //!< 境界ボックス
		};

	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief シーン用メッシュ描画機能を生成する
		explicit MeshRenderFeature(RenderScene& scene) {

		}

		//! @brief メッシュコマンドを処理して描画パケットを構築する
		void update(UpdateContext& context);

		//! @brief メッシュインスタンスを追加する
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
			Vector<MeshProxy*> proxies; //!< 所属プロキシ
		};
		//! @brief RenderView ごとのメッシュ描画データ
		struct MeshViewData {
			//! @brief CPU 側メッシュ描画データ
			struct CPUData {
				//! @brief メッシュ単位の CPU 描画データ
				struct Unit {
					//! @brief LOD ごとのインスタンス範囲
					struct LOD {
						Vector<s32> instances; //!< インスタンスインデックス
						s32 instanceStart; //!< インスタンス開始位置
						s32 instanceCount; //!< インスタンス数
					};
					Vector<LOD> lods; //!< LOD 一覧
				};
				HashMap<MeshUnitKey,Unit,MeshUnitKeyHasher> units; //!< メッシュ単位データ
				Vector<bool> culls; //!< カリング結果
				Vector<s8>   lods; //!< 選択 LOD
				Vector<s32>  instances; //!< インスタンス一覧
				Vector<MaterialDrawPacket> packets; //!< 描画パケット
			} cpu;

			//! @brief GPU 側メッシュ描画データ
			struct GPUData {
				Ref<RHI::Buffer> instances; //!< インスタンスバッファ

				//! @brief CPU 側データを GPU バッファへ反映する
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
