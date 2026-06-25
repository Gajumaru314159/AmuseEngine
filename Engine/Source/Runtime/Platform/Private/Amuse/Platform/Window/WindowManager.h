//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Platform/Window.h>

//============================================
// クラス宣言
//============================================
namespace Amuse {
    //! @brief  ウィンドウマネージャ
    //! @ingroup AmusePlatform
    class WindowManager {
    public:
        static WindowManager* Get();
    public:

        //! @brief  コンストラクタ
        WindowManager();

        //! @brief  デストラクタ
        ~WindowManager();

        //! @brief  メインウィンドウを設定
        void setMainWindow(const Ref<Window>&);

        //! @brief  メインウィンドウを取得
        auto getMainWindow()->Ref<Window>;

        //! @brief  メインウィンドウが生成済みか
        auto hasMainWindow() const -> bool;

    private:

        Ref<Window> m_mainWindow;

    };

}
