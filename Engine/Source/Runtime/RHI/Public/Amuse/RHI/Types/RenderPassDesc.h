//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Constants.h>

namespace Amuse {
    // enum class AttachmentLoadOp {
    //     Load,
    //     Clear,
    //     DontCare,
    // };
    // enum class AttachmentStoreOp {
    //     Store,
    //     DontCare,
    // };

    //! @ingroup AmuseRHI
    enum class RenderPassFlag {
        AllowUAVWrite       = get_bit(0),   //!< RenderPass内でのUAVへの書き込みを許可する
        SuspendingPass      = get_bit(1),   //!< 中断中のレンダーパスである
        ResumingPass        = get_bit(2),   //!< 再開するレンダーパスである
    };
	using RenderPassFlags = BitFlags<RenderPassFlag>;


    //! @ingroup AmuseRHI
    enum class RenderPassBeforeAccessType {
        Discard,                // 以前のコンテンツに依存していない。
        Preserve,               // 以前のコンテンツに依存している
        Clear,                  // リソースをクリアする必要がある
        NoAccess,               // 読み取りも書き込みもしない。
    };

    //! @ingroup AmuseRHI
    enum class RenderPassAfterAccessType {
        Discard,                // このRenderPass中二書き込んだデータに将来の依存関係がない。
        Preserve,               // 以降のコンテンツが依存している
        NoAccess,               // 読み取りも書き込みもしない。 TODO Vulkanとの対応関係を確認
    };

    //! @brief レンダーパスのカラーターゲット設定
    //! @ingroup AmuseRHI
    struct RenderPassColorTargetDesc {
        Ref<RenderTexture> texture; //!< テクスチャ
		RenderPassBeforeAccessType beforeAccess; //!< 遷移前アクセス状態
		RenderPassAfterAccessType afterAccess; //!< 遷移後アクセス状態
    };

    //! @brief レンダーパスのターゲット設定
    //! @ingroup AmuseRHI
    struct RenderPassTargetDesc {
        Ref<RenderTexture> texture; //!< テクスチャ
        RenderPassBeforeAccessType beforeAccess; //!< 遷移前アクセス状態
        RenderPassAfterAccessType afterAccess; //!< 遷移後アクセス状態
    };
    using RenderPassTargetDescArray = FixedVector<RenderPassTargetDesc, RENDER_TARGET_MAX>;

    //! レンダーパス定義
    //! @ingroup AmuseRHI
    struct BeginPassParam {
        RenderPassTargetDescArray colors; //!< カラーターゲット一覧
        RenderPassTargetDesc depth; //!< 深度値
        RenderPassTargetDesc stencil; //!< ステンシル値
        RenderPassFlags flags; //!< フラグ
    };

}
