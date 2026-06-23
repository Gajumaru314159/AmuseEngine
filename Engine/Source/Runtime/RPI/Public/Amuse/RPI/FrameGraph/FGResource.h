//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse::RPI {

	//! @brief FrameGraphで使用されるリソースの種類
	enum class FGResourceType : s32 {
		Invalid,
		Texture,
		Buffer,
		Dummy,
	};

	//! @brief フレームグラフリソースを表す構造体
	struct FGResource {
		FGResourceType type = FGResourceType::Invalid; //!< リソース種別
		s32 value = -1; //!< リソースID

		//! @brief 有効なリソースかを判定する
		operator bool()const {
			return type != FGResourceType::Invalid;
		}
	};

}
