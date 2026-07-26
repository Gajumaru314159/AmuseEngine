//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Platform/Window.h>
#include <Amuse/Core/Thread/Thread.h>

using namespace Amuse;

TEST(Window, ShowHide) {

    WindowDesc desc;
    auto window = Window::Create(desc);

    window->show();
    window->moveToCenter();

    Thread::Sleep(1000);

    window->maximize();
    ASSERT_TRUE(window->getState().has(WindowState::Maximized));

    window->minimize();
    ASSERT_TRUE(window->getState().has(WindowState::Minimized));

    window->restoreSize();

}

TEST(Window, Title) {

    WindowDesc desc;
    desc.title = "AAA";
    auto window = Window::Create(desc);
    ASSERT_EQ(window->getTitle(), "AAA");

    window->setTitle("BBB");
    ASSERT_EQ(window->getTitle(), "BBB");

}

TEST(Window, PosSize) {

    WindowDesc desc;
    desc.clientSize = { 512, 256 };
    auto window = Window::Create(desc);
    ASSERT_EQ(window->getSize(), Vec2(512,256));

    window->setSize({ 800, 600 });
    ASSERT_EQ(window->getSize(), Vec2(800,600));


    window->setPosition({ 123,250 });
    ASSERT_EQ(window->getPosition(), Vec2(123, 250));

    window->setPosition({ 456,250 });
    ASSERT_EQ(window->getPosition(), Vec2(456, 250));

}
