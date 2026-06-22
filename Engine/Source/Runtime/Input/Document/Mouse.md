# Mouse {#Mouse}

## 概要

Mouseは、マウスボタン、ホイール、座標、移動量をButtonまたはAxisとして提供する静的アクセサクラスです。
ボタン入力はButton API、連続値はAxis API、座標取得は専用関数で扱います。

## 機能要件

- 左、右、中、拡張ボタンをButtonとして参照できること。
- ホイール、座標、移動量をAxisとして参照できること。
- 現在のマウスポインタ座標とフレーム間移動量を取得できること。
- `MouseButton`または`MouseAxis`から入力オブジェクトを取得できること。

## 基本設計

```cpp
if (Input::Mouse::Left.down()) {
    Vec2 pos = Input::Mouse::GetPos();
    LOG_INFO("Click: {}, {}", pos.x, pos.y);
}

f32 wheel = Input::Mouse::Wheel.value();
Vec2 delta = Input::Mouse::GetDeltaPos();
```

## API

```cpp
//! @brief マウスボタンからButtonを取得する
//! @param key マウスボタン
static constexpr Button GetButton(MouseButton key);

//! @brief マウス軸からAxisを取得する
//! @param axis マウス軸
static constexpr Axis GetAxis(MouseAxis axis);

//! @brief マウスポインタの座標を取得する
static Vec2 GetPos();

//! @brief マウスポインタの移動量を取得する
static Vec2 GetDeltaPos();
```

## 主な入力

- ボタン: `Left`、`Right`、`Middle`、`X1`、`X2`、`X3`、`X4`
- 軸: `Wheel`、`WheelX`、`X`、`Y`、`DeltaX`、`DeltaY`

## 実装上の注意点

- 座標系はウィンドウシステムとの連携に依存します。必要に応じてPlatformのWindow座標変換を併用します。
- `DeltaX`と`DeltaY`は更新単位でリセットされるフレーム差分として扱います。
- マウスデバイスが無効な構成では、状態取得は既定値を返します。
