project (DirectX12RHI)

set(pch_header "Amuse/DirectX12RHI/pch.h")

# 依存ライブラリ設定
set(public_libs 
    Core
    RHI
)
set(private_libs 
    DirectXTex
    d3d12.lib
    dxgi.lib
    dxguid.lib
    "${AMUSE_THIRD_PARTY_PATH}/PIX/bin/WinPixEventRuntime.lib"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/lib/x64/dxcompiler.lib"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/lib/x64/dxil.lib"
    "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/lib/x64/dstorage.lib"
)
list(APPEND copy_files
    "${AMUSE_THIRD_PARTY_PATH}/PIX/bin/WinPixEventRuntime.dll"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxcompiler.dll"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxil.dll"
    "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/bin/x64/dstorage.dll"
    "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/bin/x64/dstoragecore.dll"
)

# インクルードディレクトリ設定
set(public_includes
    "${DirectXTex_SOURCE_DIR}/DirectXTex"
    "${AMUSE_THIRD_PARTY_PATH}/PIX/include"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/inc"
    "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/include"
)
set(private_includes "")

amuse_add_library()
