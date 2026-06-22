# ButtonAndAxis {#ButtonAndAxis}

## 概要

ButtonとAxisは、入力デバイスの種類に依存しない入力参照オブジェクトです。
Buttonは押下状態を持つ入力、Axisは連続値を持つ入力を表します。

## 機能要件

- キーボードキー、マウスボタン、ゲームパッドボタンを同じButton APIで扱えること。
- マウス移動量、ホイール、スティックなどを同じAxis APIで扱えること。
- 現在状態をポーリングで取得できること。
- 入力状態に応じたイベントコールバックをバインドできること。

## 基本設計

```cpp
if (Keyboard::Space.down()) {
    LOG_INFO("Jump");
}

ButtonHandle jumpHandle;
Keyboard::Space.bindDown(jumpHandle, [] {
    LOG_INFO("Jump");
});

AxisHandle wheelHandle;
Mouse::Wheel.bind(wheelHandle, [](f32 value) {
    LOG_INFO("Wheel: {}", value);
});
```

## Button API

```cpp
//! @brief 指定状態かを取得する
//! @param state 入力状態
bool is(ButtonState state) const;

//! @brief 押された瞬間かを取得する
bool down() const;

//! @brief 離された瞬間かを取得する
bool up() const;

//! @brief 押されているかを取得する
bool pressed() const;

//! @brief 離されているかを取得する
bool released() const;

//! @brief 押された瞬間のイベントをバインドする
bool bindDown(ButtonHandle& handle, const ButtonDelegate& func) const;

//! @brief 離された瞬間のイベントをバインドする
bool bindUp(ButtonHandle& handle, const ButtonDelegate& func) const;
```

## Axis API

```cpp
//! @brief 軸の現在値を取得する
f32 value() const;

//! @brief 軸入力イベントをバインドする
bool bind(AxisHandle& handle, const AxisDelegate& func) const;
```

## 実装上の注意点

- ButtonとAxisは`DeviceID`、入力コード、ユーザー番号を保持する軽量オブジェクトです。
- 実際の状態取得とイベント登録は、`InputModule`が保持する`IInputDevice`に委譲します。
- `ButtonHandle`と`AxisHandle`は、バインド解除の寿命管理に使用します。
