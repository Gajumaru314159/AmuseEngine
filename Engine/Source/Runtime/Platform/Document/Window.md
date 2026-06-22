# Window {#Window}

## 概要

Windowは、ネイティブウィンドウの生成、状態取得、座標変換、イベント通知を扱う抽象クラスです。
利用側は`Window::Create()`でウィンドウを生成し、実体は対象OSごとの実装クラスが担当します。

## 機能要件

- タイトル、クライアントサイズ、表示状態、ウィンドウモードを指定して生成できること。
- 表示、閉じる、最大化、最小化、中央移動、サイズ復元を操作できること。
- 位置、サイズ、モード、スタイル、フォーカスなどの状態を取得・変更できること。
- クライアント座標とスクリーン座標を相互変換できること。
- ネイティブハンドルを必要な低レベル連携に渡せること。
- ウィンドウイベントをリスナとして購読できること。

## 基本設計

```cpp
Platform::WindowDesc desc;
desc.title = "Amuse";
desc.clientSize = { 1280, 720 };
desc.show = true;

Ref<Platform::Window> window = Platform::Window::Create(desc);
window->addEventListener(handle, [](const Platform::WindowEventArgs& args) {
    if (args.type == Platform::WindowEventType::Close) {
        LOG_INFO("Window closed");
    }
});
```

## API

```cpp
//! @brief ウィンドウを生成する
//! @param desc ウィンドウ生成設定
//! @return 生成したウィンドウ
static Ref<Window> Create(const WindowDesc& desc);

//! @brief ウィンドウを表示する
virtual void show() = 0;

//! @brief ウィンドウを閉じる
virtual void close() = 0;

//! @brief ウィンドウ状態を取得する
virtual WindowStates getState() const = 0;

//! @brief クライアント座標をスクリーン座標に変換する
virtual Vec2 getScreenPoint(const Vec2& clientPoint) const = 0;

//! @brief スクリーン座標をクライアント座標に変換する
virtual Vec2 getClientPoint(const Vec2& screenPoint) const = 0;

//! @brief ネイティブハンドルを取得する
virtual void* getHandle() const = 0;

//! @brief ウィンドウイベントのリスナを追加する
virtual void addEventListener(WindowEventHandle& handle, WindowEventNotifier::delegate_type func) = 0;
```

## データ設計

```cpp
struct WindowDesc {
    String title;
    Vec2 clientSize;
    WindowMode mode;
    bool resizable;
    bool show;
};
```

## 実装上の注意点

- `Window`は`RefObject`を継承するため、所有は`Ref<Window>`で管理します。
- `getHandle()`の戻り値型はプラットフォームごとに異なります。Windowsでは`HWND`を想定します。
- `isValid()`がfalseになったウィンドウは、システム側から閉じられた状態として扱います。
