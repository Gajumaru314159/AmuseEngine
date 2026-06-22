//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Types/DescriptorLayoutDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief バインドレスリソースハンドル
	struct BindlessHandle {
		BindingType type = BindingType::Unknown; //!< 種別
		u32			index = 0; //!< インデックス
		u32			reserved0 = 0; //!< 予約領域0
		u32			reserved1 = 0; //!< 予約領域1


		//! @brief 等値比較を行う
		bool operator==(const BindlessHandle& other) const {
			return memcmp(this, &other, sizeof(BindlessHandle)) == 0;
		}

		//! @brief 非等値比較を行う
		bool operator!=(const BindlessHandle& other) const {
			return !(*this == other);
		}
	};

}
