#------------------------------------------------------------------------------
# ThirdPartyオプション設定
#------------------------------------------------------------------------------
# mimalloc
set(MI_BUILD_SHARED OFF)
set(MI_BUILD_TESTS OFF)
set(MI_MALLOC_OVERRIDE OFF)
set(MI_BUILD_OBJECT OFF)
set(MI_USE_CXX ON)
# tracy
#set(TRACY_ENABLE ON)
#set(TRACY_ON_DEMAND ON)
#set(TRACY_DELAYED_INIT ON)
#set(TRACY_MANUAL_LIFETIME ON)
#set(TRACY_DBGHELP_LOCK DbgHelp)
# DirectXTex
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
# GoogleTest
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST CACHE BOOL OFF "")
if(WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "")
endif()
set(FG_BUILD_TEST OFF)

# glslang
set(BUILD_SHARED_LIBS OFF)

# assimp
set(ASSIMP_BUILD_TESTS FALSE)
set(ASSIMP_INSTALL FALSE)
set(ASSIMP_BUILD_ASSIMP_TOOLS FALSE)
set(ASSIMP_BUILD_ZLIB ON)
set(P2T_STATIC_EXPORTS TRUE)


#------------------------------------------------------------------------------
# ローダー
#------------------------------------------------------------------------------
include(FetchContent)

function(amuse_fetch_git name url)
    FetchContent_Declare(${name} GIT_REPOSITORY ${url})
    FetchContent_MakeAvailable(${name})
endfunction()

function(amuse_fetch_git_with_tag name url tag)
    FetchContent_Declare(${name} GIT_REPOSITORY ${url} GIT_TAG ${tag})
    FetchContent_MakeAvailable(${name})
endfunction()

#------------------------------------------------------------------------------
# ThirdPartyライブラリ
#------------------------------------------------------------------------------
amuse_fetch_git_with_tag(tracy https://github.com/wolfpld/tracy "v0.13.0")
amuse_fetch_git_with_tag(mimalloc https://github.com/microsoft/mimalloc.git "v3.1.5")
amuse_fetch_git_with_tag(GSL https://github.com/microsoft/GSL.git "v4.0.0")
amuse_fetch_git_with_tag(utf8cpp https://github.com/nemtrif/utfcpp.git "v4.0.5")
amuse_fetch_git_with_tag(magic_enum https://github.com/Neargye/magic_enum.git "v0.8.0")
amuse_fetch_git_with_tag(googletest https://github.com/google/googletest.git "v1.15.2")
amuse_fetch_git_with_tag(assimp https://github.com/assimp/assimp.git "v6.0.5")
amuse_fetch_git(FrameGraph https://github.com/skaarj1989/FrameGraph.git)

if(AMUSE_DIRECTX AND WIN32)
    amuse_fetch_git_with_tag(DirectXTex https://github.com/microsoft/DirectXTex.git "sept2023")
endif()

if(AMUSE_VULKAN)
    #amuse_fetch_git_with_tag(spirv https://github.com/KhronosGroup/SPIRV-Cross.git "sdk-1.3.231.1")
endif()

if(UNIX)
    amuse_fetch_git_with_tag(glfw https://github.com/glfw/glfw.git "3.4")
endif()