project(Core)

# 依存ライブラリ
set(public_libs
    GSL
    utf8cpp
    magic_enum
    TracyClient
)
set(private_libs
    mimalloc-static
)
if (WIN32)
    # AsyncFileおよびCompressで使用するライブラリ
    list(APPEND private_libs
        "${AMUSE_THIRD_PARTY_PATH}/GDeflate/GDeflate.lib"
        "${AMUSE_THIRD_PARTY_PATH}/GDeflate/deflate.lib"
        "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/lib/x64/dstorage.lib"
    )
    
    list(APPEND copy_files
        "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/bin/x64/dstorage.dll"
        "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/bin/x64/dstoragecore.dll"
    )
endif()
if (UNIX)
    # UUIDで使用するライブラリ
    list(APPEND private_libs uuid stdc++_libbacktrace)
endif()


# 追加のインクルードディレクトリ
set(public_includes 
    "${AMUSE_THIRD_PARTY_PATH}/nlohmann"
)

set(private_includes)
if (WIN32)
    list(APPEND private_includes 
        "${AMUSE_THIRD_PARTY_PATH}/GDeflate" 
        "${AMUSE_THIRD_PARTY_PATH}/DirectStorage/native/include"
    )
endif()

amuse_add_library()