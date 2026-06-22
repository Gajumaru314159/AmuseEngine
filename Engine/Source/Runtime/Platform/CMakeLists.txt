project (Platform)

set(pch_header "Amuse/Platform/pch.h")

# インクルードディレクトリ設定
set(public_includes "")
set(private_includes "")

# 依存ライブラリ設定
set(public_libs Core)
set(private_libs "")

if(UNIX)
    find_package(X11 REQUIRED)
    list(APPEND public_libs X11::X11 glfw)
endif()

amuse_add_library()
