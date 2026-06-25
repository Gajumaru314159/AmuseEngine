//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Misc/BlobView.h>
#include <Amuse/RHI/Types/Barrier.h>
#include <Amuse/RHI/Types/CommandParam.h>

namespace Amuse {
	//! @brief  バッファコピー範囲
	//! @ingroup AmuseRHI
	struct BufferCopyRegion {
		Ref<Buffer> source;		//!< コピー元
		Ref<Buffer> target;		//!< コピー先
		u64 sourceOffset = 0;	//!< コピー元オフセット
		u64 targetOffset = 0;	//!< コピー先オフセット
		u64 size = 0;			//!< コピーサイズ。0の場合はコピー可能な残り全体
	};

	//! @brief  テクスチャコピー範囲
	//! @ingroup AmuseRHI
	struct TextureCopyRegion {
		Ref<Texture> source;						//!< コピー元
		Ref<Texture> target;						//!< コピー先
		TextureSubresource sourceSubresource;		//!< コピー元サブリソース
		TextureSubresource targetSubresource;		//!< コピー先サブリソース
		Point sourceOrigin;							//!< コピー元原点
		Point targetOrigin;							//!< コピー先原点
		Size size;									//!< コピーサイズ
	};

	//! @brief  バッファ/テクスチャコピー範囲
	//! @ingroup AmuseRHI
	struct BufferTextureCopyRegion {
		Ref<Buffer> buffer;							//!< バッファ
		Ref<Texture> texture;						//!< テクスチャ
		u64 bufferOffset = 0;						//!< バッファオフセット
		u32 rowPitch = 0;							//!< 1行のバイト数
		u32 depthPitch = 0;							//!< 1スライスのバイト数
		TextureSubresource textureSubresource;		//!< テクスチャサブリソース
		Point textureOrigin;						//!< テクスチャ原点
		Size size;									//!< コピーサイズ
	};

}
