//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/Core/Template/Container/IndexedVector.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
#include <Amuse/RPI/Material/MaterialPropertiesDesc.h>
#include <Amuse/RPI/Material/Material.h>
namespace Amuse {

	//! @brief 平行光源データ
	//! @ingroup AmuseRPI
	struct DirectionalLightData {
		Color color; //!< 色
		Vec3 direction; //!< 方向
		f32  intensity; //!< 強度
	};
	//! @brief 点光源データ
	//! @ingroup AmuseRPI
	struct PointLightData {
		Color color; //!< 色
		Vec3 position; //!< 位置
		f32  intensity; //!< 強度
	};
	//! @brief スポットライトデータ
	//! @ingroup AmuseRPI
	struct SpotLightData {
		Color color; //!< 色
		Vec3 position; //!< 位置
		f32  intensity; //!< 強度
		Vec3 direction; //!< 方向
		f32  anguler; //!< 角度
	};

	//! @brief 要素数に応じて拡張する GPU バッファ
	//! @ingroup AmuseRPI
	class ResizableBuffer {
	public:
		//! @brief バッファ生成設定
		struct Desc {
			s32 stride; //!< 要素バイト幅
		};
	public:
		//! @brief 要素バイト幅を指定して生成する
		ResizableBuffer(const Desc& desc)
			: m_desc(desc), m_size(0)
		{
			resize(16);
		}

		//! @brief バッファ容量を変更する
		void resize(size_t size) {
			m_size = size;
			m_buffer = Buffer::Create(BufferDesc::ByteAddress(m_desc.stride * m_size));
		}

		//! @brief 要素データを GPU バッファへ転送する
		void update(size_t count, const void* data) {
			if (m_size <= count) {
				resize(count * 2);
			}
			m_buffer->update(m_desc.stride * count, data);
		}

		//! @brief GPU バッファを取得する
		Ref<Buffer> getBuffer() const {
			return m_buffer;
		}
	private:
		Desc m_desc;
		Ref<Buffer> m_buffer;
		size_t m_size;
	};


	//! @brief      マテリアル描画機能
	//! @ingroup AmuseRPI
	class PointLightRenderFeature : public RenderFeature {
	public:
		//! @brief 点光源描画で使用するマテリアルプロパティを取得する
		static MaterialPropertiesSetDesc GetProperties() {
			MaterialPropertiesSetDesc desc;
			desc.scene.buffers = {
				"PointLight"
			};
			return desc;
		}
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief 点光源描画機能を生成する
		PointLightRenderFeature()
			: m_buffer({ sizeof(PointLightData) })
		{

		}

		//! @brief 点光源スロットを確保する
		s32 acquire() {
			return m_point.push();
		}
		//! @brief 点光源スロットを解放する
		void release(s32 index) {
			m_point.erase(index);
		}
		//! @brief 点光源データを設定する
		void set(s32 index, const PointLightData& data) {
			if (index < 0 || index >= m_point.size()) {
				LOG_FATAL("PointLightRenderFeature::set: Invalid index {}", index);
				return;
			}
			m_point.at(index) = data;
		}

		//! @brief 点光源バッファを更新してシーンブロックへ設定する
		void render(FG& fg,RenderScene& scene) {
			m_buffer.update(m_point.size(), m_point.data());

			if (auto feature = scene.findFeature<MaterialRenderFeature>()) {
				feature->getSceneBlock().setBuffer("PointLight", m_buffer.getBuffer());
			}
		}

	private:
		IndexedVector<PointLightData> m_point;
		ResizableBuffer m_buffer;
	};

}
