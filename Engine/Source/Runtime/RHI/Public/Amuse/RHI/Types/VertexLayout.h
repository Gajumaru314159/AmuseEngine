//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Constants.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief		型
	//! 
	//! @see		VertexAttribute
	//! @see		VertexLayout
	enum class ElementType : u32 {
		Int8,
		Int16,
		Int32,
		UInt8,
		UInt16,
		UInt32,
		Float,
		Int8Norm,
		UInt8Norm,
	};

	inline s32 GetSize(ElementType type) {
		switch (type)	
		{
		case Amuse::RHI::ElementType::Int8:		return 1;
		case Amuse::RHI::ElementType::Int16:		return 2;
		case Amuse::RHI::ElementType::Int32:		return 4;
		case Amuse::RHI::ElementType::UInt8:		return 1;
		case Amuse::RHI::ElementType::UInt16:		return 2;
		case Amuse::RHI::ElementType::UInt32:		return 4;
		case Amuse::RHI::ElementType::Float:		return 4;
		case Amuse::RHI::ElementType::Int8Norm:	return 1;
		case Amuse::RHI::ElementType::UInt8Norm:	return 1;
		default: return 0;
		}
	}


	//! @brief		セマンティクス
	//! 
	//! @details	Positionは頂点内で1度しか使用できません。
	//! @see		VertexAttribute
	//! @see		VertexLayout
	enum class Semantic : u32 {
		Position,		//!< 頂点
		Normal,			//!< 法線
		Binormal,		//!< 従法線
		Tangent,		//!< 接線
		Color,			//!< 色
		TexCoord,		//!< テクスチャ座標(UV座標)
		BlendIndices,	//!< ブレンド・インデックス
		BlendWeights,	//!< ブレンドの重み
		PointSize,		//!< ポイントサイズ
	};

	enum class VertexInputRate : u32 {
		Vertex,			//!< 頂点ごと
		Instance,		//!< インスタンスごと
	};

	//! @brief  頂点属性
	//! @see    VertexLayout
	struct VertexAttribute {

		Semantic		semantic;	//!< セマンティクス
		s32				offset;		//!< 頂点構造体内でのオフセット
		ElementType		type;		//!< コンポーネント型
		s32				dimension;	//!< 次元数
		s32				index;		//!< セマンティクス内インデックス
		VertexInputRate inputRate = VertexInputRate::Vertex;	//!< 頂点ごとかインスタンスごとか

	public:

		//! @brief      コンストラクタ
		VertexAttribute() = default;

		//! @brief      コンストラクタ
		VertexAttribute(Semantic semantic, s32 offset, ElementType type, s32 dimension = 1, s32 index = 0)
			:semantic(semantic), offset(offset), type(type), dimension(dimension), index(index) {}

	};

	//! @brief  頂点属性・配列
	using VertexAttributeArray = FixedVector<VertexAttribute, VERTEX_ATTRIBUTE_MAX>;


	//! @brief  頂点レイアウト
	struct VertexLayoutDesc {
		String				 name;				//!< 名前
		VertexAttributeArray attributes;			//!< 属性リスト
		s32					 vertexStride = 0;		//!< ストライド幅
		s32					 instanceStride = 0;	//!< ストライド幅
	};

}
