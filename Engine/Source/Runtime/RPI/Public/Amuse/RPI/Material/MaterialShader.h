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

namespace Amuse::RPI {

	//! @brief  マテリアルシェーダー
	//! @details パス定義やルートシグネチャ、パイプラインキャッシュなどマテリアル間で共有できる情報を保持します。
	class MaterialShader : public RefObject
	{
	public:
		using PipelineState = Amuse::RHI::PipelineState;
		using RootSignature = Amuse::RHI::RootSignature;
	public:
		//! @brief  生成
		static Ref<MaterialShader> Create(const MaterialDesc& desc);

		const MaterialDesc& getDesc()const { return m_desc; }
		const MaterialBlockDesc& getBlockDesc()const { return m_blockDesc; }
		const Ref<RootSignature>& getRootSignature()const { return m_signature; }

		s32 calcQualityIndex(StringView pass, s32 quality) const;
		bool prepare(const Ref<RHI::VertexLayout>& layout);
		Ref<PipelineState> getPipeline(const Ref<RHI::VertexLayout>& layout, StringView pass, s32 quality);

	private:
		MaterialShader(const MaterialDesc& desc);

	private:
		struct PipelineKey {
			String pass;
			s32 qualityIndex;
			Ref<RHI::VertexLayout> layout;
			bool operator==(const PipelineKey& rhs)const { return pass == rhs.pass && qualityIndex == rhs.qualityIndex && layout == rhs.layout; }
			bool operator!=(const PipelineKey& rhs)const { return !(*this == rhs); }
		};
		struct PipelineKeyHasher {
			size_t operator()(const PipelineKey& v)const
			{
				return std::hash<decltype(v.pass)>{}(v.pass) ^ std::hash<decltype(v.qualityIndex)>{}(v.qualityIndex) ^ (size_t)(v.layout.get());
			}
		};
		using PipelineMap = HashMap<PipelineKey, Ref<PipelineState>, PipelineKeyHasher>;

		Ref<PipelineState> createPipeline(const PipelineKey& key, const ShaderSet& shaders, const Ref<RHI::VertexLayout>& vertexLayout);

	private:
		MaterialDesc		m_desc;
		MaterialBlockDesc	m_blockDesc;
		Ref<RootSignature>  m_signature;

		SpinLock		    m_pipelinesLock;
		PipelineMap	        m_pipelines;

	};

}
