//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/DescriptorTable.h>
#include <Amuse/RHI/Types/TextureDesc.h>

namespace Amuse {
	//! @brief  デスクリプタ・テーブル・コマンドパラメータ
	//! @ingroup AmuseRHI
	struct SetDescriptorTableParam {
		Ref<DescriptorTable> table; //!< デスクリプタテーブル
		s32				slot; //!< ルートスロット
	public:
		//! @brief 空のパラメータを生成する
		SetDescriptorTableParam() = default;
		//! @brief デスクリプタテーブルとルートスロットを指定して生成する
		SetDescriptorTableParam(const Ref<DescriptorTable>& table,s32 slot)
			:table(table),slot(slot)
		{}
	};


	//! @brief  CommandList::draw() で指定するパラメータ
	//! @ingroup AmuseRHI
	struct DrawParam {
		u32 startVertex = 0;	//!< 頂点開始インデックス
		u32 vertexCount = 0;	//!< 描画頂点数
		u32 startInstance = 0;	//!< インスタンス開始インデックス
		u32 instanceCount = 1;	//!< インスタンス数
	};


	//! @brief  CommandList::drawIndexed() で指定するパラメータ
	//! @ingroup AmuseRHI
	struct DrawIndexedParam {
		u32 startVertex = 0;	//!< 頂点開始インデックス
		u32 startIndex = 0;		//!< インデックス開始インデックス
		u32 indexCount = 0;		//!< 描画頂点数
		u32 startInstance = 0;	//!< インスタンス開始インデックス
		u32 instanceCount = 1;	//!< インスタンス数
	};

	//! @brief  CommandList::dispatch() で指定するパラメータ
	//! @ingroup AmuseRHI
	struct DispatchParam {
		u32 groupX = 1;	//!< X方向のスレッドグループ数
		u32 groupY = 1;	//!< Y方向のスレッドグループ数
		u32 groupZ = 1;	//!< Z方向のスレッドグループ数
	};


	//! @brief  Indirectコマンドで指定するパラメータ
	//! @ingroup AmuseRHI
	struct IndirectCommandParam {
		Ref<Buffer> argumentBuffer;	//!< 引数バッファ
		u64 argumentOffset = 0;		//!< 引数バッファ内のオフセット
		u32 commandCount = 1;		//!< 実行するコマンド数
	};

	//! @brief  テクスチャ内のコピー対象範囲
	//! @ingroup AmuseRHI
	struct TextureSubresource {
		u32 arrayIndex = 0; //!< 配列インデックス
		u32 faceIndex = 0; //!< キューブ面インデックス
		u32 mipLevel = 0; //!< Mip レベル
		std::optional<IntRect> region; //!< 矩形範囲
	};

	//! @brief  TextureSubresourceから実配列レイヤを取得
	inline u32 GetTextureSubresourceArrayLayer(const TextureDesc& desc, const TextureSubresource& subresource) {
		return desc.type == TextureType::Cube ? subresource.arrayIndex * 6 + subresource.faceIndex : subresource.arrayIndex;
	}


	//! @brief  テクスチャ間コピーのパラメータ
	//! @ingroup AmuseRHI
	struct CopyTextureParam {
		TextureSubresource src;		//!< コピー元
		TextureSubresource dest;	//!< コピー先
	};


	//! @brief  ルート定数設定のパラメータ
	//! @ingroup AmuseRHI
	struct SetRootConstantsParam {
		s32			offset;	//!< 書き込むオフセット
		BlobView	blob;	//!< 書き込むバイナリ
	};

}
