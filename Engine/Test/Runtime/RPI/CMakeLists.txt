project (RPI-test)

set(pch_header "pch.h")

# 依存ライブラリ設定
set(public_libs Core RPI)
set(private_libs "")

if(AMUSE_DIRECTX AND WIN32)
    list(APPEND private_libs DirectX12RHI)
endif()

if(AMUSE_VULKAN)
    list(APPEND private_libs VulkanRHI)
endif()

amuse_add_test()
