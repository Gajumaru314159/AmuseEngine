# Input {#Input}

## 概要

Inputモジュールは、キーボード、マウス、ゲームパッドなどの入力デバイスを共通のボタン入力と軸入力として扱うための機能を提供します。
利用側はデバイス実装を直接参照せず、`Button`、`Axis`、`Keyboard`、`Mouse`などの公開APIから入力状態の取得とイベントバインドを行います。

入力デバイスの実体は`IInputDevice`として実装し、`InputModule`が更新と検索を管理します。

# サブページ

- \subpage ButtonAndAxis
- \subpage Keyboard
- \subpage Mouse
- \subpage DeviceAndModule
