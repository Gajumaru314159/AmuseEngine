# DeviceAndModule {#DeviceAndModule}

## 概要

IInputDeviceは入力デバイス実装の共通インターフェイスです。
InputModuleはデバイスの生成、保持、更新、検索を担当し、ButtonとAxisからの問い合わせを実際のデバイスへ橋渡しします。

## 機能要件

- 複数種類の入力デバイスを同じインターフェイスで管理できること。
- デバイスIDとユーザー番号の組み合わせでデバイスを検索できること。
- 毎フレーム入力状態を更新できること。
- Button/Axisのポーリングとイベントバインドをデバイス実装へ委譲できること。
- キーボードとマウスの使用有無を設定できること。

## 基本設計

```cpp
Input::InputConfig config;
config.useKeyboard = true;
config.useMouse = true;

Input::InputModule inputModule(&config, windowManager);

while (running) {
    inputModule.update();
}
```

## IInputDevice API

```cpp
//! @brief デバイスIDを取得する
virtual DeviceID getDeviceId() const = 0;

//! @brief 入力状態を更新する
virtual void update();

//! @brief ボタンの入力状態を取得する
//! @param code デバイス内の入力コード
virtual ButtonStates getButtonStates(u32 code) const;

//! @brief 軸の入力値を取得する
//! @param code デバイス内の入力コード
virtual f32 getAxisValue(u32 code) const;

//! @brief ボタン入力イベントをバインドする
virtual bool bindButton(u32 code, ButtonState state, ButtonHandle& handle, const ButtonDelegate& func);

//! @brief 軸入力イベントをバインドする
virtual bool bindAxis(u32 code, AxisHandle& handle, const AxisDelegate& func);
```

## InputModule API

```cpp
//! @brief 入力モジュールを初期化する
//! @param config 入力設定
//! @param windowManager ウィンドウ管理
InputModule(InputConfig* config, Platform::WindowManager& windowManager);

//! @brief 入力状態を更新する
void update();

//! @brief 入力デバイスを検索する
//! @param id デバイスID
//! @param user ユーザー番号
IInputDevice* findDevice(DeviceID id, u32 user);
```

## データ設計

```cpp
struct InputConfig {
    bool useKeyboard = true;
    bool useMouse = true;
};
```

## 実装上の注意点

- `DeviceID`は文字列ハッシュから生成し、`AMUSE_DECL_INPUT_DEVICE_ID()`で各デバイスに定義します。
- `InputModule`は`Singleton<InputModule>`として参照されるため、生成順と破棄順に注意します。
- ゲームパッドなどの追加デバイスは`IInputDevice`を実装し、Button/Axisの問い合わせを処理します。
