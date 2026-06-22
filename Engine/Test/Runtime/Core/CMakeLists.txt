project (Core-test)

set(pch_header "pch.h")

# 依存ライブラリ設定
set(public_libs Core)
set(private_libs "")

# インクルードディレクトリ設定
set(public_includes "")
set(private_includes "")

amuse_add_test()
