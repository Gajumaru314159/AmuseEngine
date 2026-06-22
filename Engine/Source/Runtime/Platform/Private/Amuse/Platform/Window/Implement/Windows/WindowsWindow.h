//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#ifdef OS_WINDOWS
#include <Windows.h>
#include <Amuse/Platform/Window.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;

    //! @brief  ウィンドウ
    class WindowsWindow : public Window {
    public:

        WindowsWindow(const WindowDesc& params);
        virtual ~WindowsWindow()override;


        //===============================================================
        // ウィンドウ操作
        //===============================================================
        void show()override;
        void close()override;

        void maximize()override;
        void minimize()override;
        void moveToCenter()override;
        void restoreSize()override;


        //===============================================================
        // 情報取得
        //===============================================================

        bool isValid()const noexcept override;
        bool isMainWindow()const override;
        WindowStates getState()const override;

        Vec2 getScreenPoint(const Vec2& clientPoint)const override;
        Vec2 getClientPoint(const Vec2& screenPoint)const override;

        void setTitle(StringView getTitle) override;
        const String& getTitle()const override;

        void setPosition(Vec2 position) override;
        Vec2 getPosition()const noexcept override;

        void setSize(Vec2 size) override;
        Vec2 getSize()const override;

        void setMode(WindowMode mode) override;
        WindowMode getMode()const override;

        WindowStyle getStyle()const override;
        void setStyle(WindowStyle style) override;

        void* getHandle()const override;
        String getTextInput() override{
            auto text = std::move(m_inputText);
            m_inputText.clear();
            return text;
        }

        //===============================================================
        // イベント
        //===============================================================

        void addEventListener(WindowEventHandle& handle, WindowEventNotifier::delegate_type func) override;

    public:

        //! @brief              HWNDを取得
        HWND getHWND() { return m_hWnd; }

    private:

        //! @brief              ウィンドウごとの Window Proceduer
        LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


    public:

        //! @brief              ウィンドウ位置を画面の中央にする
        static LRESULT CALLBACK staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


    private:

        static Atomic<s32>      m_windowNum;        //!< 生成されたウィンドウの数

        WindowMode              m_mode;             //!< 表示モード

        String                  m_title;            //!< タイトル
        HWND                    m_hWnd;             //!< ウィンドウハンドル
        HWND                    m_hParentWnd;       //!< 親ウィンドウハンドル
        const StringBase<TCHAR> m_className;        //!< ウィンドウクラス名
        s32                     m_windowID;         //!< ウィンドウごとに割り当てられるID
        String                  m_inputText;        //!< 入力テキストバッファ

        Vec2    m_lastSize;
        bool    m_isSizing = false;
        bool    m_handled = false;

        WindowEventNotifier m_notifier;

    };

}
#endif // OS_WINDOWS
