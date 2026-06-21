cmake_minimum_required (VERSION 3.21)
project ("AmuseEngine")

#------------------------------------------------------------------------------
# オプション定義
#------------------------------------------------------------------------------
option(AMUSE_DIRECTX "Enable DirectX RHI" ON)
option(AMUSE_VULKAN "Enable Vulkan RHI" ON)
option(AMUSE_TEST "Enable UnitTest" ON)

option(AMUSE_TRACY "Enable Tracy Profiler" OFF)


#------------------------------------------------------------------------------
# パス設定
#------------------------------------------------------------------------------
set(AMUSE_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(AMUSE_ENGINE_PATH "${AMUSE_ROOT_PATH}/Engine")
set(AMUSE_ENGINE_BUILD_PATH "${AMUSE_ENGINE_PATH}/Build")
set(AMUSE_ENGINE_BINARIES_PATH "${AMUSE_ENGINE_PATH}/Binaries")
set(AMUSE_ENGINE_SOURCE_PATH "${AMUSE_ENGINE_PATH}/Source")
set(AMUSE_THIRD_PARTY_PATH "${AMUSE_ENGINE_SOURCE_PATH}/ThirdParty")

#------------------------------------------------------------------------------
# ソリューション設定
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)                                                  # C++23を使用
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                         # 古いSTLにフォールバックしない
set(CMAKE_CXX_EXTENSIONS OFF)                                               # GNU拡張オフ
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" )                             # DebugとReleaseのみ使用(MinSizeRel等を使わない)
set(CMAKE_DEBUG_POSTFIX "")                                                 # Debug関連のポストフィクスにdをつけない
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${AMUSE_ENGINE_BINARIES_PATH}")         # ライブラリの出力先
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${AMUSE_ENGINE_BINARIES_PATH}")         # 実行ファイルの出力先
set(BUILD_TESTING OFF)                                                      # CTest関連のテストが生成さ得れるのを防ぐ
set_property(GLOBAL PROPERTY USE_FOLDERS ON)                                # IDEでフィルタを使用する
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMake")            # CMake関係のプロジェクトを_CMakeにまとめる


#------------------------------------------------------------------------------
# ThirdPartyライブラリの追加
#------------------------------------------------------------------------------
add_subdirectory("Engine/Source/ThirdParty")
include("CMake/ThirdParty.cmake")

#------------------------------------------------------------------------------
# 機能ロード
#------------------------------------------------------------------------------
include("CMake/Compiler.cmake")
include("CMake/Module.cmake")


#------------------------------------------------------------------------------
# GoogleTestを有効化（最上位ディレクトリで呼ぶ必要あり）
#------------------------------------------------------------------------------
if(AMUSE_TEST)
    enable_testing()
    include(GoogleTest)
endif()


#------------------------------------------------------------------------------
# プロジェクト読み込み
#------------------------------------------------------------------------------
add_subdirectory("Engine/Source/Runtime")
add_subdirectory("Engine/Test/Runtime")