//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
#pragma region Enum

	//! @brief      フィル・モード
	//! 
	//! @details	ポリゴン描画時にどこを塗りつぶすか
	//! @ingroup AmuseRHI
	enum class FillMode :u32 {
		Solid,		//<! 全面
		Wireframe,	//<! 辺のみ
		Point,		//<! 頂点のみ
	};


	//! @brief      カリング方法
	//! @ingroup AmuseRHI
	enum class CullMode :u32 {
		None,		//!< なし
		Front,		//!< 前面を非表示
		Back,		//!< 背面を非表示 
	};

#pragma endregion

	//! @brief      ラスタライズ定義
	//! @ingroup AmuseRHI
	struct RasterizerDesc{
		FillMode fillMode{ FillMode::Solid };	//!< フィル・モード
		CullMode cullMode{ CullMode::Back };	//!< カリング方法
	};

}