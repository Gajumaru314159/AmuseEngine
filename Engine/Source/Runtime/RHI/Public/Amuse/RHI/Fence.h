//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  コマンドキュー種別
	enum class CommandQueueType {
		Graphics,	//!< グラフィックスキュー
		Compute,	//!< コンピュートキュー
		Copy,		//!< コピーキュー
	};

	//! @brief  フェンス定義
	struct FenceDesc {
		String name;			//!< 名前
		u64 initialValue = 0;	//!< 初期値
	};

	//! @brief  GPU同期フェンス
	class Fence : public GraphicObject {
	public:

		//! @brief  フェンスを作成
		static Ref<Fence> Create(const FenceDesc& desc = {});

	public:

		//! @brief  GPUで完了している値を取得
		virtual u64 getCompletedValue() const = 0;

		//! @brief  指定値までCPUで待機
		virtual void wait(u64 value) = 0;

	};

}
