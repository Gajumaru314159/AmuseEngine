project (RPI)

set(pch_header "Amuse/RPI/pch.h")

# 依存ライブラリ設定
set(public_libs RHI ImGui FrameGraph)
set(private_libs assimp)

amuse_add_library()