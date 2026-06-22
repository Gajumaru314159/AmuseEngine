//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Platform/Window.h>
#include <Amuse/Platform/Window/WindowManager.h>
#include <Amuse/Platform/Window/Implement/Windows/WindowsWindow.h>
#include <Amuse/Platform/Window/Implement/Linux/LinuxWindow.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;

    //! @brief      ウィンドウを生成
    Ref<Window> Window::Create(const WindowDesc& desc) {
#include <Amuse/Core/Core.h>
#ifdef OS_WINDOWS
        auto window = Ref<Window>(new WindowsWindow(desc));
#elif defined(OS_LINUX)
        auto window = Ref<Window>(new LinuxWindow(desc));
#else
        static_assert(false, "Window is not implemented.");
#endif

        if (!window->isValid()) {
            return nullptr;
        }

        if (auto manager = WindowManager::Get()) {
            if (!manager->hasMainWindow()) {
                manager->setMainWindow(window);
            }
        }

        return window;
    }

    //! @brief      デストラクタ
    Window::~Window() {
    }


    //! @brief      メインウィンドウを取得
    Ref<Window> Window::Main() {
        // static変数を直接使用するとDLL間で共有できない
        if (auto manager = WindowManager::Get()) {
            return manager->getMainWindow();
        }
        return nullptr;
    }

}
