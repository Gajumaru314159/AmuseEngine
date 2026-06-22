project (VulkanRHI)

set(pch_header "Amuse/VulkanRHI/pch.h")

# 追加の依存ライブラリ
set(public_libs 
    Core
    RHI
)
set(private_libs)
if(WIN32)
    list(APPEND private_libs
        "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib"
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/lib/x64/dxcompiler.lib"
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/lib/x64/dxil.lib"
    )
    list(APPEND copy_files
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxcompiler.dll"
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxil.dll"
    )
endif()
if(UNIX)
    find_package(Vulkan REQUIRED)
    list(APPEND private_libs
        Vulkan::Vulkan
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Linux/lib/libdxcompiler.so"
        "${AMUSE_THIRD_PARTY_PATH}/dxc/Linux/lib/libdxil.so"
    )
endif()

# インクルードディレクトリ設定
set(public_includes "$ENV{VK_SDK_PATH}/Include")
set(private_includes "")
list(APPEND public_includes "${CMAKE_CURRENT_SOURCE_DIR}/Private" "${AMUSE_THIRD_PARTY_PATH}/stb/include")
if(WIN32)
    list(APPEND public_includes "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/inc")
endif()
if(UNIX)
    list(APPEND public_includes "${AMUSE_THIRD_PARTY_PATH}/dxc/Linux/include/dxc")
endif()

set(copy_files
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxcompiler.dll"
    "${AMUSE_THIRD_PARTY_PATH}/dxc/Windows/bin/x64/dxil.dll"
)

amuse_add_library()
