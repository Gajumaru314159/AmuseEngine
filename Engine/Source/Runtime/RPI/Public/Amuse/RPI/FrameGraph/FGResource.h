//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse {

	//! @brief FrameGraphで使用されるリソースの種類
	//! @ingroup AmuseRPI
	enum class FGResourceType : s32 {
		Invalid,
		Texture,
		Buffer,
		Dummy,
	};

	//! @brief フレームグラフリソースを表す構造体
	//! @ingroup AmuseRPI
	struct FGResource {
		FGResourceType type = FGResourceType::Invalid; //!< リソース種別
		s32 value = -1; //!< リソースID

		//! @brief 有効なリソースかを判定する
		operator bool()const {
			return type != FGResourceType::Invalid;
		}
	};

}
