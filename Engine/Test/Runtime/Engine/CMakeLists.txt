project (Engine-test)

set(pch_header "pch.h")

# 依存ライブラリ設定
set(public_libs Core RPI EnTT)
set(private_libs "")

amuse_add_test()
