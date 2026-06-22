add_subdirectory("Core")
add_subdirectory("Entry")
add_subdirectory("Platform")
add_subdirectory("Input")
add_subdirectory("RHI")

if(AMUSE_DIRECTX AND WIN32)
    add_subdirectory("DirectX12RHI")
endif()

if(AMUSE_VULKAN)
    add_subdirectory("VulkanRHI")
endif()
