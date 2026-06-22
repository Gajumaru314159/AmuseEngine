# System {#System}

## 概要

Systemは、プラットフォーム全体の初期化、更新、システム情報取得を担当します。
Platformモジュールのサービス登録口として`RegisterPlatformService()`を提供し、DI経由で他モジュールから利用できる状態を作ります。

## 機能要件

- Platformモジュールのサービスを`ServiceInjector`に登録できること。
- OS依存の初期化と更新をモジュール単位で実行できること。
- 実行環境の言語設定を取得できること。

## 基本設計

```cpp
ServiceInjector injector;
Platform::RegisterPlatformService(injector);

if (!Platform::System::Setup()) {
    return false;
}

while (running) {
    Platform::System::Update();
}
```

## API

```cpp
//! @brief Platformモジュールのサービスを登録する
//! @param injector サービス登録先
void RegisterPlatformService(ServiceInjector& injector);

//! @brief プラットフォームの言語を取得する
//! @return 言語設定
Language GetLanguage();

//! @brief プラットフォーム機能を初期化する
//! @return 成功した場合true
bool Setup();

//! @brief プラットフォーム機能を更新する
//! @return 継続可能な場合true
bool Update();
```

## 実装上の注意点

- `Setup()`はウィンドウシステムや入力連携など、Platformが持つOS資源の準備に使用します。
- `Update()`はOSイベントのポーリングやウィンドウ状態の反映を行うため、メインループから定期的に呼び出します。
- 言語設定はOSごとのロケール取得処理を隠蔽し、`Language`で返します。
