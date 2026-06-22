# Keyboard {#Keyboard}

## 概要

Keyboardは、キーボード入力をButtonとして提供する静的アクセサクラスです。
各キーは`Keyboard::Space`や`Keyboard::A`のような定数として定義され、直接状態取得やイベントバインドに使用できます。

## 機能要件

- 主要なファンクションキー、英数字キー、記号キー、矢印キー、修飾キーをButtonとして参照できること。
- `Key`列挙値からButtonを取得できること。
- キーボードデバイスの実装差を公開APIから隠蔽すること。

## 基本設計

```cpp
if (Input::Keyboard::Escape.down()) {
    requestExit();
}

Input::ButtonHandle handle;
Input::Keyboard::Space.bindDown(handle, [] {
    LOG_INFO("Space down");
});

Input::Button enter = Input::Keyboard::GetButton(Input::Key::Enter);
```

## API

```cpp
//! @brief キーからButtonを取得する
//! @param key キーコード
//! @return キーに対応するButton
static constexpr Button GetButton(Key key);
```

## 主な入力

- ファンクションキー: `F1`から`F12`
- 数字キー: `Alpha0`から`Alpha9`
- 英字キー: `A`から`Z`
- 矢印キー: `UpArrow`、`DownArrow`、`RightArrow`、`LeftArrow`
- 制御キー: `Escape`、`Space`、`Tab`、`Backspace`、`Delete`、`Enter`
- 修飾キー: `Shift`、`Ctrl`、`Alt`、`Command`、`Windows`

## 実装上の注意点

- 物理キーと文字入力は別概念です。文字列入力やIMEはPlatformの`TextInput`を使用します。
- `Keyboard`はデバイスIDとして`"Keyboard"`のハッシュ値を使用します。
- OS固有のキーコード変換はデバイス実装側で`Key`へ正規化します。
