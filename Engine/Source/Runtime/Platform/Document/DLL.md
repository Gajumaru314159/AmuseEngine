# DLL {#DLL}

## 概要

DLLは、動的ライブラリを読み込み、ライブラリ内の関数を取得・呼び出しするためのRAIIクラスです。
WindowsではDLL、Linuxでは共有オブジェクト、macOSではdylibを想定し、利用側は拡張子を意識せずにライブラリ名を指定します。

## 機能要件

- 拡張子を除いたパスから、対象プラットフォームの動的ライブラリを読み込めること。
- 関数名から関数ポインタを取得できること。
- 戻り値と引数型を指定して関数を呼び出せること。
- DLLオブジェクト破棄時にライブラリを解放すること。

## 基本設計

```cpp
DLL dll("Plugins/SamplePlugin");

auto create = dll.getFunction("CreatePlugin");
if (create) {
    void* instance = create.call<void*>();
}

dll.tryCall("ShutdownPlugin");
```

## API

```cpp
//! @brief 動的ライブラリを読み込む
//! @param path 拡張子を除いた動的ライブラリのパス
DLL(StringView path);

//! @brief 動的ライブラリを解放する
~DLL();

//! @brief 動的ライブラリから関数を取得する
//! @param name 関数名
//! @return 関数オブジェクト
Function getFunction(StringView name);

//! @brief 関数名を指定して戻り値なし関数を呼び出す
//! @param name 関数名
//! @param args 引数
//! @return 呼び出しに成功した場合true
template<typename... Args>
bool tryCall(StringView name, Args&&... args);

//! @brief 読み込み中の動的ライブラリのパスを取得する
//! @return 拡張子付きパス。読み込みに失敗している場合は空文字列。
const String& getPath() const;
```

## 実装上の注意点

- DLLから生成されたインスタンスは、DLLが解放される前に破棄する必要があります。
- `Function::call()`は関数シグネチャを検証できません。呼び出し側が正しい戻り値型と引数型を指定します。
- 頻繁に呼ぶ関数は`tryCall()`ではなく`getFunction()`で取得して保持します。
