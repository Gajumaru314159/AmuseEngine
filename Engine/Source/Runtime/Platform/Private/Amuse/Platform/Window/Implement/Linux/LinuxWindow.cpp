//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Core.h>
#ifdef OS_LINUX
#include <Amuse/Platform/Window/Implement/Linux/LinuxWindow.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Amuse/Platform/Window/WindowManager.h>

namespace Amuse {
    namespace {

        class X11Context {
        public:
            static X11Context& Instance() {
                static X11Context instance;
                return instance;
            }

            ::Display* display() const { return m_display; }
            int screen() const { return m_screen; }
            ::Window root() const { return m_root; }

        private:
            X11Context() {
                m_display = XOpenDisplay(nullptr);
                if (!m_display) {
                    LOG_ERROR("XOpenDisplay に失敗しました。DISPLAY 環境変数を確認してください。");
                    return;
                }
                m_screen = XDefaultScreen(m_display);
                m_root = XRootWindow(m_display, m_screen);
            }

            ~X11Context() {
                if (m_display) {
                    XCloseDisplay(m_display);
                    m_display = nullptr;
                }
            }

            ::Display* m_display = nullptr;
            int m_screen = 0;
            ::Window m_root = 0;
        };

    } // namespace

    LinuxWindow::LinuxWindow(const WindowDesc& desc) {
        createWindow(desc);
    }

    LinuxWindow::~LinuxWindow() {
        destroyWindow();
    }

    void LinuxWindow::createWindow(const WindowDesc& desc) {
        auto& context = X11Context::Instance();
        if (!context.display()) {
            return;
        }

        m_title = desc.title.empty() ? "OctbitEngine" : String(desc.title);
        m_mode = desc.mode;
        m_resizable = desc.resizable;
        m_clientSize = desc.clientSize;
        m_restoreSize = desc.clientSize;

        unsigned int width = static_cast<unsigned int>(desc.clientSize.x > 0 ? desc.clientSize.x : 1280);
        unsigned int height = static_cast<unsigned int>(desc.clientSize.y > 0 ? desc.clientSize.y : 720);

        ::Window window = XCreateSimpleWindow(
            context.display(),
            context.root(),
            0,
            0,
            width,
            height,
            0,
            0,
            0);

        if (!window) {
            LOG_ERROR("XCreateSimpleWindow に失敗しました。");
            return;
        }

        m_native.display = context.display();
        m_native.window = window;

        XStoreName(m_native.display, m_native.window, m_title.c_str());
        XFlush(m_native.display);

        if (desc.show) {
            show();
        }
    }

    void LinuxWindow::destroyWindow() {
        if (m_native.display && m_native.window) {
            XDestroyWindow(m_native.display, m_native.window);
            XFlush(m_native.display);
            m_native.window = 0;
        }
    }

    void LinuxWindow::show() {
        if (!m_native.window) return;
        XMapRaised(m_native.display, m_native.window);
        XFlush(m_native.display);
        m_visible = true;
        updateState(WindowState::Minimized, false);
        updateState(WindowState::Focused, true);

        WindowEventArgs args{};
        args.type = WindowEventType::Show;
        m_notifier.invoke(args);
    }

    void LinuxWindow::close() {
        if (m_closed) return;
        m_closed = true;
        m_visible = false;
        updateState(WindowState::Minimized, false);
        updateState(WindowState::Maximized, false);
        updateState(WindowState::Focused, false);
        WindowEventArgs args{};
        args.type = WindowEventType::Close;
        m_notifier.invoke(args);
        destroyWindow();
    }

    void LinuxWindow::maximize() {
        if (!m_native.window) return;

        auto& context = X11Context::Instance();
        Vec2 size{
            static_cast<f32>(XDisplayWidth(context.display(), context.screen())),
            static_cast<f32>(XDisplayHeight(context.display(), context.screen()))
        };

        m_restoreSize = m_clientSize;
        setPosition({0.0f, 0.0f});
        setSize(size);
        updateState(WindowState::Maximized, true);
        updateState(WindowState::Minimized, false);
    }

    void LinuxWindow::minimize() {
        if (!m_native.window) return;
        auto& context = X11Context::Instance();
        XIconifyWindow(m_native.display, m_native.window, context.screen());
        XFlush(m_native.display);
        updateState(WindowState::Maximized, false);
        updateState(WindowState::Minimized, true);
        m_visible = false;

        WindowEventArgs args{};
        args.type = WindowEventType::Hide;
        m_notifier.invoke(args);
    }

    void LinuxWindow::moveToCenter() {
        auto& context = X11Context::Instance();
        Vec2 size = getSize();
        Vec2 position{
            static_cast<f32>((XDisplayWidth(context.display(), context.screen()) - static_cast<int>(size.x)) / 2),
            static_cast<f32>((XDisplayHeight(context.display(), context.screen()) - static_cast<int>(size.y)) / 2)
        };
        setPosition(position);
    }

    void LinuxWindow::restoreSize() {
        updateState(WindowState::Minimized, false);
        updateState(WindowState::Maximized, false);
        if (m_mode == WindowMode::FullScreen) {
            applyFullscreen(false);
        } else {
            setSize(m_restoreSize);
        }
        show();
    }

    bool LinuxWindow::isValid()const noexcept {
        return !m_closed && m_native.window != 0;
    }

    bool LinuxWindow::isMainWindow()const {
        if (auto manager = WindowManager::Get()) {
            return manager->hasMainWindow() && manager->getMainWindow()->getHandle() == getHandle();
        }
        return false;
    }

    WindowStates LinuxWindow::getState()const {
        return m_states;
    }

    Vec2 LinuxWindow::getScreenPoint(const Vec2& clientPoint)const {
        updateWindowGeometryCache();
        return clientPoint + m_position;
    }

    Vec2 LinuxWindow::getClientPoint(const Vec2& screenPoint)const {
        updateWindowGeometryCache();
        return screenPoint - m_position;
    }

    void LinuxWindow::setTitle(StringView title) {
        m_title = title;
        if (!m_native.window) return;
        XStoreName(m_native.display, m_native.window, m_title.c_str());
        XFlush(m_native.display);
    }

    const String& LinuxWindow::getTitle()const {
        return m_title;
    }

    void LinuxWindow::setPosition(Vec2 position) {
        if (!m_native.window) return;
        m_position = position;
        XMoveWindow(m_native.display, m_native.window, static_cast<int>(position.x), static_cast<int>(position.y));
        XFlush(m_native.display);
    }

    Vec2 LinuxWindow::getPosition()const noexcept {
        updateWindowGeometryCache();
        return m_position;
    }

    void LinuxWindow::setSize(Vec2 size) {
        if (!m_native.window) return;
        m_clientSize = size;
        if (!m_states.has(WindowState::Minimized) && m_mode != WindowMode::FullScreen) {
            m_restoreSize = size;
        }
        XResizeWindow(m_native.display, m_native.window,
                      static_cast<unsigned int>(size.x),
                      static_cast<unsigned int>(size.y));
        XFlush(m_native.display);
    }

    Vec2 LinuxWindow::getSize()const {
        updateWindowGeometryCache();
        return m_clientSize;
    }

    void LinuxWindow::setMode(WindowMode mode) {
        if (m_mode == mode) return;
        if (mode == WindowMode::FullScreen) {
            applyFullscreen(true);
        } else {
            applyFullscreen(false);
        }
        m_mode = mode;
    }

    WindowMode LinuxWindow::getMode()const {
        return m_mode;
    }

    WindowStyle LinuxWindow::getStyle()const {
        return m_style;
    }

    void LinuxWindow::setStyle(WindowStyle style) {
        m_style = style;
    }

    void* LinuxWindow::getHandle()const {
        return const_cast<NativeWindowHandle*>(&m_native);
    }

    String LinuxWindow::getTextInput() {
        return {};
    }

    void LinuxWindow::addEventListener(WindowEventHandle& handle, WindowEventNotifier::delegate_type func) {
        m_notifier.add(handle, func);
    }

    void LinuxWindow::applyFullscreen(bool enable) {
        auto& context = X11Context::Instance();
        if (enable) {
            updateState(WindowState::FullScreen, true);
            setPosition({0.0f, 0.0f});
            setSize({
                static_cast<f32>(XDisplayWidth(context.display(), context.screen())),
                static_cast<f32>(XDisplayHeight(context.display(), context.screen()))
            });
        } else {
            updateState(WindowState::FullScreen, false);
            setSize(m_restoreSize);
            moveToCenter();
        }
    }

    void LinuxWindow::updateState(WindowState state, bool enable) {
        if (enable) {
            m_states.on(state);
        } else {
            m_states.off(state);
        }
    }

    void LinuxWindow::updateWindowGeometryCache() const {
        if (!m_native.display || !m_native.window) return;

        XWindowAttributes attrs{};
        if (XGetWindowAttributes(m_native.display, m_native.window, &attrs)) {
            m_clientSize = {
                static_cast<f32>(attrs.width),
                static_cast<f32>(attrs.height)
            };
        }

        auto& context = X11Context::Instance();
        ::Window child;
        int screenX = 0;
        int screenY = 0;
        if (context.display() &&
            XTranslateCoordinates(
                m_native.display,
                m_native.window,
                context.root(),
                0,
                0,
                &screenX,
                &screenY,
                &child)) {
            m_position = {
                static_cast<f32>(screenX),
                static_cast<f32>(screenY)
            };
        }
    }

} // namespace Amuse

#endif // OS_LINUX
