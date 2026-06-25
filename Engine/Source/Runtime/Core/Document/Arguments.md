# Arguments {#Arguments}

## 概要

Argumentsは、アプリケーション起動時のコマンドライン引数を取得するためのクラスです。
OSごとに異なる引数取得処理をCoreモジュール内で吸収し、利用側には`Vector<String>`に近い読み取りAPIを提供します。

## 機能要件

- 起動時に渡された引数を順序付きで参照できること。
- 引数数、空判定、範囲指定アクセスを提供すること。
- 範囲forで読み取り走査できること。
- 引数の所有権はArgumentsが持ち、利用側は`String`として参照できること。

## 基本設計

```cpp
Arguments args;

if (!args.empty()) {
    const String& first = args.at(0);
}

for (const String& arg : args) {
    LOG_INFO("arg: {}", arg);
}
```

## API

```cpp
//! @brief コマンドライン引数を取得して初期化する
Arguments();

//! @brief 指定インデックスの引数を取得する
//! @param index インデックス
//! @return 引数文字列
const String& at(s32 index) const;

//! @brief 先頭要素を指す読み取り専用イテレータを取得する
const_iterator begin() const noexcept;

//! @brief 末尾の次を指す読み取り専用イテレータを取得する
const_iterator end() const noexcept;

//! @brief 引数の数を取得する
s32 size() const noexcept;

//! @brief 引数が空かを取得する
bool empty() const noexcept;
```

## 実装上の注意点

- `at()`は範囲外アクセス時にCoreコンテナ側の挙動に従います。
- 引数の文字コード変換は各OS実装側で行い、公開APIでは`String`として扱います。
- Linuxではエントリ関数でキャッシュした`argc`/`argv`を参照して引数を取得します。
- 引数を変更するAPIは提供しません。
