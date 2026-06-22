//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;

    //! @brief  ウィンドウ・イベント・タイプ
    enum class WindowEventType {
        Unknown,    //!< 不明

        Move,       //!< ウィンドウが移動
        Size,       //!< サイズ変更

        Minimize,   //!< 最小化
        Maximize,   //!< 最大化

        Activate,   //!< アクティブ状態に変更
        Deactivate, //!< 非アクティブ状態に変更

        Enable,     //!< 入力有効化
        Disable,    //!< 入力無効化

        Show,       //!< 表示
        Hide,       //!< 非表示

        DragEnter,  //!< ドラッグアイテムがウィンドウ内に入った
        DragDrop,   //!< ドラッグアイテムがドロップされた

        Close,      //!< 終了時
        Destroy,    //!< 破棄
    };

    class Draggable;

    //! @brief ウィンドウイベント引数
    struct WindowEventArgs {

        WindowEventType type; //!< 種別

        // Move
        Vec2 oldPos; //!< 変更前の位置
        Vec2 newPos; //!< 変更後の位置

        // Size
        bool isSizing; //!< サイズ変更中フラグ
        Vec2 oldSize; //!< 変更前のサイズ
        Vec2 newSize; //!< 変更後のサイズ

        // Drag
        Draggable* item=nullptr; //!< 対象項目
    };



    //! @brief  ウィンドウ・イベント
    //! @{
    using WindowEventNotifier = EventNotifier<const WindowEventArgs&>;
    using WindowEventHandle = typename WindowEventNotifier::Handle;
    //! @}

}
