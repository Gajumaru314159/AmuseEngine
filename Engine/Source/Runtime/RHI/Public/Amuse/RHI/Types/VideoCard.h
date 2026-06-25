//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Misc/MemorySize.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse {
	//! @brief 映像出力モード
	//! @ingroup AmuseRHI
	struct VideoOutputMode {
		u32				width;		//!< 幅
		u32				height;		//!< 高さ
		f32				refreshRate;//!< リフレッシュレート
		TextureFormat	format;		//!< フォーマット
	};

	//! @brief 映像出力情報
	//! @ingroup AmuseRHI
	struct VideoOutput {
		String	name;		//!< 名前
		IntRect	rect;		//!< 出力領域
		bool	isPrimary;  //!< プライマリフラグ
		VideoOutputMode			currentMode;	//!< 現在のモード
		Vector<VideoOutputMode> modes;			//!< モードリスト
	};

	//! @brief ビデオカード情報
	//! @ingroup AmuseRHI
	struct VideoCard {
		String				name;		//!< 名前
		MemorySize			memory;		//!< メモリサイズ
		u32					deviceId;	//!< デバイスID
		Vector<VideoOutput> outputs;	//!< 出力先
	};

}
