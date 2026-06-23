//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/Material/MaterialDesc.h>

namespace Amuse::RPI {

	//! @brief 頂点レイアウト識別子
	enum class VertexLayoutId : s32 {};

	//! @brief マテリアルプロパティ種別
	enum class MaterialPropertyType {
		Texture,
		Buffer,
		Matrix,
		Vector,
		Scalar,
		Integer,
	};

	//! @brief マテリアル値プロパティの配置情報
	struct MaterialValuePropertyDesc {
		MaterialPropertyType type; //!< プロパティ種別
		s32 offset = -1; //!< バッファ内バイトオフセット
		s32 index = -1; //!< リソースインデックス
		s32 slot = -1; //!< デスクリプタスロット
	};

	using MaterialPropertyMap = Map<String, MaterialValuePropertyDesc, std::less<>>;

}
