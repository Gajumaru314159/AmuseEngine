//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/RHI/Types/TextureDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  テクスチャサブリソース範囲
	struct TextureSubresourceRange {
		static constexpr u32 All = static_cast<u32>(-1); //!< All

		u32 baseMipLevel = 0;		//!< 開始Mip
		u32 mipLevelCount = All;	//!< Mip数。Allの場合は残り全て
		u32 baseArrayLayer = 0;		//!< 開始配列スライス
		u32 arrayLayerCount = All;	//!< 配列スライス数。Allの場合は残り全て
	};

	//! @brief  テクスチャ遷移バリア
	struct TextureBarrier {
		Ref<Texture> texture;							//!< 対象テクスチャ
		TextureSubresourceRange range;					//!< 対象範囲
		TextureState before = TextureState::Unknown;	//!< 遷移前状態
		TextureState after = TextureState::Unknown;		//!< 遷移後状態
	};

	//! @brief  バッファ遷移バリア
	struct BufferBarrier {
		Ref<Buffer> buffer;								//!< 対象バッファ
		BufferState before = BufferState::Unknown;		//!< 遷移前状態
		BufferState after = BufferState::Unknown;		//!< 遷移後状態
	};

}
