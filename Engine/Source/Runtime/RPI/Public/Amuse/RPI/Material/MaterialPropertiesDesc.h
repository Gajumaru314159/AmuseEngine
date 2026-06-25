//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse {

	//! @brief マテリアルプロパティ名の集合
	//! @ingroup AmuseRPI
	struct MaterialPropertiesDesc {
		Vector<String>  textures; //!< テクスチャ名
		Vector<String>  buffers; //!< バッファ名
		Vector<String>  matrices; //!< 行列プロパティ名
		Vector<String>  vectors; //!< ベクトルプロパティ名
		Vector<String>  scalars; //!< スカラー名
		Vector<String>  integers; //!< 整数名

		//! @brief 別の MaterialPropertiesDesc を各プロパティリストの後に追加する
		//! @retval true 成功
		//! @retval false プロパティ名の重複が含まれていない
		bool merge(const MaterialPropertiesDesc& other);
	};

	//! @brief グローバル・シーン・ビュー別のマテリアルプロパティ集合
	//! @ingroup AmuseRPI
	struct MaterialPropertiesSetDesc {
		MaterialPropertiesDesc global; //!< グローバルプロパティ
		MaterialPropertiesDesc scene; //!< シーンプロパティ
		MaterialPropertiesDesc view; //!< ビュープロパティ

		//! @brief 別の MaterialPropertiesSetDesc を各プロパティリストの後に追加する
		bool merge(const MaterialPropertiesSetDesc& other);
	};

}
