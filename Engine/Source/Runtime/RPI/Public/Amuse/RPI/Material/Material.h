//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/Material/MaterialDesc.h>
#include <Amuse/RPI/Material/MaterialBlock.h>

namespace Amuse::RPI {

	class MaterialShader;
	struct MaterialBlockSet;

	//! @brief  マテリアル
	class Material : public RefObject {
	public:
		using Texture = Amuse::RHI::Texture;
		using Sampler = Amuse::RHI::Sampler;
		using Buffer = Amuse::RHI::Buffer;
	public:

		//! @brief  生成
		static Ref<Material> Create(const Ref<MaterialShader>& shader);
		static Ref<Material> Create(const MaterialDesc& desc);

	public:

		const MaterialDesc& getDesc()const;
		const Ref<MaterialShader>& getShader()const { return m_shader; }

		//! @brief  マテリアルパラメータが存在するか
		bool hasProperty(StringView name, MaterialPropertyType type)const;
		bool hasTexture(StringView name)const;  //!< @copybrief hasProperty()
		bool hasBuffer(StringView name)const;   //!< @copybrief hasProperty()
		bool hasMatrix(StringView name)const;   //!< @copybrief hasProperty()
		bool hasVector(StringView name)const;   //!< @copybrief hasProperty()
		bool hasScalar(StringView name)const;   //!< @copybrief hasProperty()
		bool hasInteger(StringView name)const;  //!< @copybrief hasProperty()

		//! @brief  マテリアルパラメータを設定
		void setTexture(StringView name, const Ref<Texture>& value);
		void setBuffer(StringView name, const Ref<Buffer>& value);
		void setMatrix(StringView name, const Matrix& value);
		void setVector(StringView name, Color value);
		void setScalar(StringView name, f32 value);
		void setInteger(StringView name, f32 value);

		s32 calcQualityIndex(StringView pass, s32 quality) const;

		//! @brief  GPUリソースの事前生成
		//! @details GPUリソースを事前生成しておくことで描画時のスパイクを回避することができます。
		bool prepare(const Ref<Mesh>& mesh);

		void record(Ref<RHI::CommandList>& commandList, MaterialBlockSet& blocks, const Ref<Mesh>& mesh, s32 submesh, StringView pass, s32 quality = 0);

		auto& getBlock() { return m_block;}
	public:

		//! @brief  グローバルマテリアルパラメータを設定
		//! @{
		static void SetGlobalTexture(StringView name, const Ref<Texture>& value);
		static void SetGlobalBuffer(StringView name, const Ref<Buffer>& value);
		static void SetGlobalMatrix(StringView name, const Matrix& value);
		static void SetGlobalVector(StringView name, Color value);
		static void SetGlobalScalar(StringView name, f32 value);
		static void SetGlobalInteger(StringView name, s32 value);
		//! @}

	private:
		Material(const Ref<MaterialShader>& shader);

	private:
		Ref<MaterialShader> m_shader;
		MaterialBlock       m_block;

	};



	inline bool Material::hasTexture(StringView name)const { return hasProperty(name, MaterialPropertyType::Texture); }
	inline bool Material::hasBuffer(StringView name)const { return hasProperty(name, MaterialPropertyType::Buffer); }
	inline bool Material::hasMatrix(StringView name)const { return hasProperty(name, MaterialPropertyType::Matrix); }
	inline bool Material::hasVector(StringView name)const { return hasProperty(name, MaterialPropertyType::Vector); }
	inline bool Material::hasScalar(StringView name)const { return hasProperty(name, MaterialPropertyType::Scalar); }
	inline bool Material::hasInteger(StringView name)const { return hasProperty(name, MaterialPropertyType::Integer); }

}
