//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/Material/MaterialDesc.h>
#include <Amuse/RPI/Material/MaterialBlock.h>

namespace Amuse {

	class MaterialShader;
	struct MaterialBlockSet;

	//! @brief  マテリアル
	//! @ingroup AmuseRPI
	class Material : public RefObject {
	public:

		//! @brief  生成
		static Ref<Material> Create(const Ref<MaterialShader>& shader);
		//! @brief マテリアル定義から生成する
		static Ref<Material> Create(const MaterialShaderDesc& desc);

	public:

		//! @brief マテリアル定義を取得する
		const MaterialShaderDesc& getDesc()const;
		//! @brief 共有シェーダを取得する
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
		//! @brief バッファパラメータを設定する
		void setBuffer(StringView name, const Ref<Buffer>& value);
		//! @brief 行列パラメータを設定する
		void setMatrix(StringView name, const Matrix& value);
		//! @brief ベクトルパラメータを設定する
		void setVector(StringView name, Color value);
		//! @brief スカラーパラメータを設定する
		void setScalar(StringView name, f32 value);
		//! @brief 整数パラメータを設定する
		void setInteger(StringView name, f32 value);

		//! @brief 指定パスで使用する品質インデックスを計算する
		s32 calcQualityIndex(StringView pass, s32 quality) const;

		//! @brief  GPUリソースの事前生成
		//! @details GPUリソースを事前生成しておくことで描画時のスパイクを回避することができます。
		bool prepare(const Ref<Mesh>& mesh);

		//! @brief 指定したメッシュサブセットの描画コマンドを記録する
		void record(Ref<CommandList>& commandList, MaterialBlockSet& blocks, const Ref<Mesh>& mesh, s32 submesh, StringView pass, s32 quality = 0);

		//! @brief マテリアル固有ブロックを取得する
		auto& getBlock() { return m_block;}
	public:

		//! @brief  グローバルマテリアルパラメータを設定
		//! @{
		static void SetGlobalTexture(StringView name, const Ref<Texture>& value);
		//! @copybrief SetGlobalTexture()
		static void SetGlobalBuffer(StringView name, const Ref<Buffer>& value);
		//! @copybrief SetGlobalTexture()
		static void SetGlobalMatrix(StringView name, const Matrix& value);
		//! @copybrief SetGlobalTexture()
		static void SetGlobalVector(StringView name, Color value);
		//! @copybrief SetGlobalTexture()
		static void SetGlobalScalar(StringView name, f32 value);
		//! @copybrief SetGlobalTexture()
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
