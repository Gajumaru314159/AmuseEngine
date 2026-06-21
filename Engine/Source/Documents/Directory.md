# ディレクトリ構造 {#Directory}

## 全体図

```
📁Engine
　📁Assets
　📁Binaries
　  📁{Platform}
　　　📄{SomeExecutable}.exe
　📁Config
　📁Intermediate
　  📁{Platform}
　📁Resource
　　📁Shaders
　📁Source
　　📁Runtime
　　　📁{モジュール名}
　　　　📁Document
　　　　📁Private
　　　　📁Public
　　　　📄CMakeLists.txt
　　📁ThirdParty
　　　📁{ライブラリ名}
　📁Test
　　📁Runtime
　　　📁{モジュール名}
　　　　📁Source
　　　　📄CMakeLists.txt
📁Sample
　📁Assets
　📁Binaries
　📁Config
　📁Intermediate
　📁Plugins
　📁Saved
　📁Source
　📄Project.json
　📄CMakeLists.txt
📁Template
　🔁Sampleと同様

```

## トップ階層

- Engine：エンジン本体
- Sample：サンプルプロジェクト
- Template：プロジェクトを作成するときのテンプレート

## セカンダリ階層

- Binaries：ビルド成果物(exe/pdb/dll)
- Intermediate：ビルドの中間生成物
- Config：プロジェクト設定のベースとなるエンジン層でのコンフィグ
- Assets：アセットブラウザに表示されるアセット群
- Source：エンジンをビルドするためのソースコード
- Test：エンジンのテストコード

## 参考

https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-directory-structure
