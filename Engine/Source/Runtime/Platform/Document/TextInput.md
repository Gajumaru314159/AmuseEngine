# TextInput {#TextInput}

## 概要

TextInputは、キーボード入力から文字列編集結果を取得するためのプラットフォーム機能です。
通常のボタン入力ではなく、IMEや編集中文字列を含むテキスト入力を扱います。

## 機能要件

- 入力済みテキストを取得できること。
- 既存テキストとカーソル位置を渡して、編集後のカーソル位置を受け取れること。
- 編集中の未確定文字列を取得できること。
- Enter、Tab、BackSpace、Delete、Ctrl入力の許可可否を指定できること。

## 基本設計

```cpp
String text = "Player";
size_t cursor = text.size();

cursor = TextInput::UpdateText(
    text,
    cursor,
    TextInputMode::Default
);

String committed = TextInput::GetText();
String editing = TextInput::GetEditingText();
```

## API

```cpp
//! @brief 確定済みの入力文字列を取得する
static String GetText();

//! @brief テキストとカーソル位置を更新する
//! @param text 現在のテキスト
//! @param cursor 現在のカーソル位置
//! @param mode 許可する編集入力
//! @return 更新後のカーソル位置
static size_t UpdateText(StringView text, size_t cursor, TextInputMode mode = TextInputMode::Default);

//! @brief テキストを更新する
//! @param text 現在のテキスト
//! @param mode 許可する編集入力
//! @return 更新後のカーソル位置
static size_t UpdateText(StringView text, TextInputMode mode = TextInputMode::Default);

//! @brief IMEなどの編集中テキストを取得する
static String GetEditingText();
```

## 実装上の注意点

- `TextInputMode::Default`はEnter、Tab、BackSpace、Deleteを許可します。
- UI側は`GetText()`の確定文字列と`GetEditingText()`の編集中文字列を分けて表示します。
- Ctrl入力を禁止する場合は`DenyCtrl`を指定します。
