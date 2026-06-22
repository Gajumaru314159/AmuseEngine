project (RHI)

set(pch_header "Amuse/RHI/pch.h")

# インクルードディレクトリ設定
set(public_includes "")
set(private_includes "")

# 依存ライブラリ設定
set(public_libs Core Platform)
set(private_libs "")

amuse_add_library()
