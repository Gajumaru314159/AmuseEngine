//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Platform/Window/WindowManager.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;

    static WindowManager* s_instance = nullptr;

    WindowManager* WindowManager::Get() {
        return s_instance;
    }

    WindowManager::WindowManager() {
        AMUSE_ASSERT(s_instance == nullptr, "{}は既に生成されています。", Type::Get<WindowManager>().name());
        s_instance = this;
    }

    WindowManager::~WindowManager() {
        s_instance = nullptr;
    }


    //! @brief  メインウィンドウを設定
    void WindowManager::setMainWindow(const Ref<Window>& window) {
        if (m_mainWindow) {
            LOG_WARNING("ウィンドウ[{}]が既にメインウィンドウに設定されているためメインウィンドウを変更できませんでした。", m_mainWindow->getTitle());
        } else {
            m_mainWindow = window;
        }
    }

    //! @brief  メインウィンドウを取得
    auto WindowManager::getMainWindow()->Ref<Window> {
        static bool isAutoGenerate = false;
        if ((!m_mainWindow || !m_mainWindow->isValid()) && isAutoGenerate==false) {
            isAutoGenerate = true;
            Platform::WindowDesc windowDesc;
            windowDesc.title = "Application";
            m_mainWindow = Platform::Window::Create(windowDesc);
        }
        return m_mainWindow;
    }

    //! @brief  メインウィンドウが生成済みか
    auto WindowManager::hasMainWindow() const -> bool {
        return m_mainWindow && m_mainWindow->isValid();
    }

}
