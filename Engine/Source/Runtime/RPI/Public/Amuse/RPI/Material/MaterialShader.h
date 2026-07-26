//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RPI/Material/MaterialBlock.h>
#include <Amuse/Core/Thread/SpinLock.h>
#include <Amuse/Core/Template/Container/Map.h>
#include <functional>

namespace Amuse {

	//! @brief  マテリアルシェーダー
	//! @details パス定義やルートシグネチャ、パイプラインキャッシュなどマテリアル間で共有できる情報を保持します。
	//! @ingroup AmuseRPI
	class MaterialShader : public RefObject
	{
	public:
		//! @brief  生成
		static Ref<MaterialShader> Create(const MaterialShaderDesc& desc);

		//! @brief マテリアル定義を取得する
		const MaterialShaderDesc& getDesc()const { return m_desc; }
		//! @brief マテリアルブロック定義を取得する
		const MaterialBlockDesc& getBlockDesc()const { return m_blockDesc; }
		//! @brief ルートシグネチャを取得する
		const Ref<RootSignature>& getRootSignature()const { return m_signature; }

		//! @brief 指定パスで使用する品質インデックスを計算する
		s32 calcQualityIndex(StringView pass, s32 quality) const;
		//! @brief 頂点レイアウトに対応するパイプラインを事前生成する
		bool prepare(const Ref<VertexLayout>& layout);
		//! @brief 頂点レイアウトとパスに対応するパイプラインを取得する
		Ref<PipelineState> getPipeline(const Ref<VertexLayout>& layout, StringView pass, s32 quality);

	private:
		MaterialShader(const MaterialShaderDesc& desc);

	private:
		//! @ingroup AmuseRPI
		struct PipelineKey {
			String pass;
			s32 qualityIndex;
			Ref<VertexLayout> layout;
			bool operator==(const PipelineKey& rhs)const { return pass == rhs.pass && qualityIndex == rhs.qualityIndex && layout == rhs.layout; }
			bool operator!=(const PipelineKey& rhs)const { return !(*this == rhs); }
		};
		//! @ingroup AmuseRPI
		struct PipelineKeyHasher {
			size_t operator()(const PipelineKey& v)const
			{
				return std::hash<decltype(v.pass)>{}(v.pass) ^ std::hash<decltype(v.qualityIndex)>{}(v.qualityIndex) ^ (size_t)(v.layout.get());
			}
		};
		using PipelineMap = HashMap<PipelineKey, Ref<PipelineState>, PipelineKeyHasher>;

		Ref<PipelineState> createPipeline(const PipelineKey& key, const ShaderSet& shaders, const Ref<VertexLayout>& vertexLayout);

	private:
		MaterialShaderDesc		m_desc;
		MaterialBlockDesc	m_blockDesc;
		Ref<RootSignature>  m_signature;

		SpinLock		    m_pipelinesLock;
		PipelineMap	        m_pipelines;

	};

}
