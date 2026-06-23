//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>

namespace Amuse::RPI {

	//! @brief      描画機能
	//! @details    O3DEでいうところのFeatureProcessor。
	//!				初期状態は非アクティブです。
	//! @ref 		RenderFeature
	class RenderFeature {
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief デストラクタ
		virtual ~RenderFeature(){}

		//! @brief      描画	
		virtual void render(FG& fg) {}
		
	};

}
