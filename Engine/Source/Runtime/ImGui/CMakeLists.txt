project (ImGui)

# インクルードディレクトリ設定
set(public_includes "${CMAKE_CURRENT_SOURCE_DIR}/Public/Amuse/ImGui/Library")
set(private_includes "${CMAKE_CURRENT_SOURCE_DIR}/Public/Amuse/ImGui/Library/misc/freetype" "${CMAKE_CURRENT_SOURCE_DIR}/Public/Amuse/ImGui/Library")

# 依存ライブラリ設定
set(public_libs Core Platform RHI Input)
set(private_libs "")

amuse_add_library() 