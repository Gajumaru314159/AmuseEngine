//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

    //! @brief このマクロの呼び出し位置の SourceLocation オブジェクトを作成する
#ifdef AMUSE_DEBUG
#define CURRENT_SOURCE_LOCATION Amuse::SourceLocation{__FILE__, FUNC_NAME ,__LINE__}
#else
#define CURRENT_SOURCE_LOCATION Amuse::SourceLocation{"","",__LINE__}
#endif

    //! @brief      ソースコード上の位置を表す構造体
    //! 
    //! @details    構築には CURRENT_SOURCE_LOCATION マクロを使用してください。
    //! @ingroup AmuseCore
    struct SourceLocation {
        const Char* filePath = nullptr;             //!< ファイルパス
        const Char* functionName = nullptr;         //!< 関数名
        s32 line = 0;                               //!< 行番号
    };


    //! @brief ログのレベル
    //! @ingroup AmuseCore
    enum class LogLevel {
        Fatal,      //!< 致命的エラー
        Error,		//!< エラー
        Warning,	//!< 警告表示
        Info,		//!< 情報表示
        Trace,		//!< デバッグ用トレース
    };


    //! @brief ログ・オブジェクト
    //! @ingroup AmuseCore
    struct Log {
        LogLevel        level = LogLevel::Trace;//!< タイプ
        const Char*     category = nullptr;     //!< カテゴリ
        const Char*     message = nullptr;      //!< メッセージ
        SourceLocation  sourceLocation;         //!< ログが発生したソースコード上の位置
    };

}