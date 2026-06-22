#------------------------------------------------------------------------------
# 指定フォルダ以下のサブディレクトリを再帰的に列挙する
# result      : 結果を格納する変数名
# root        : 検索するルートのディレクトリ 
#------------------------------------------------------------------------------
function(_amuse_get_subdirectories result root parent)
    file(GLOB children RELATIVE ${root} ${root}/${parent}/*)
    
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${root}/${child})
            list(APPEND dirlist ${child})
            _amuse_get_subdirectories(subdirs ${root} ${child})
            list(APPEND dirlist ${subdirs})
        endif()
    endforeach()
    set(${result} ${dirlist} PARENT_SCOPE)
endfunction()


#------------------------------------------------------------------------------
# CMAKE_CURRENT_SOURCE_DIR以下にあるソースコードを列挙しresultに格納する
# 対象ファイル：cpp, h, natvis
# result      : 結果を格納する変数名
#------------------------------------------------------------------------------
function(_amuse_glob_sources result)

	message("Load project [${PROJECT_NAME}]")

	# サブディレクトリを列挙
	_amuse_get_subdirectories(dirs ${CMAKE_CURRENT_SOURCE_DIR} ".")
	# ルートを追加
	list(APPEND dirs ".")

	# ソースを追加
	set(all_files "")
	foreach(dir ${dirs})
	
		set(abs_dir ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
		# ファイル検索
		file(
			GLOB
			files
			ABSOLUTE
			"${abs_dir}/*.natvis"
			"${abs_dir}/*.h"
			"${abs_dir}/*.cpp")
		
		# ソースリストを結合
		list(APPEND all_files ${files})		
		# フィルタ設定
		if(NOT ${dir} STREQUAL ".")
			source_group("${dir}" FILES ${files})
		endif()

	endforeach()
	
	if(NOT all_files)
		message(FATAL_ERROR "[${PROJECT_NAME}] One or more source files are required!")
	endif()

	# resultに格納
	set(${result} ${all_files} PARENT_SCOPE)

endfunction()





#------------------------------------------------------------------------------
# モジュールの設定
#------------------------------------------------------------------------------
function(_amuse_module_setup)

	# 依存ライブラリ
	target_link_libraries(${PROJECT_NAME} PUBLIC ${public_libs} PRIVATE ${private_libs})

	# インクルードパス
	list(APPEND public_includes "${CMAKE_CURRENT_SOURCE_DIR}/Public")   # デフォルトインクルードパス
	list(APPEND private_includes "${CMAKE_CURRENT_SOURCE_DIR}/Private") # デフォルトインクルードパス
	target_include_directories(${PROJECT_NAME} PUBLIC ${public_includes} PRIVATE ${private_includes})

    # 定義
	#list(APPEND private_definitions "${PROJECT_NAME}_API=AMUSE_API_EXPORT")
	target_compile_definitions(${PROJECT_NAME} PUBLIC ${public_definitions} PRIVATE ${private_definitions})
	
	# プリコンパイルヘッダを設定
	if(NOT pch_header AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Private/pch.h)
		set(pch_header "pch.h")
	endif()
	if(pch_header)
		set(pch_header_path "${pch_header}")

		target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${pch_header_path}>")
		if(MSVC)
			target_compile_options(${PROJECT_NAME} PRIVATE "/FI${pch_header}")
		else()
			target_compile_options(${PROJECT_NAME} PRIVATE "-include" "${pch_header}")
		endif()
	endif()
    
	# ランタイムライブラリ設定
	set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	
	# フィルタ設定
	file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${relative_path})

	# 依存ファイルのコピー
	if(copy_files)
		foreach(file IN LISTS copy_files)
			add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${file}"
					"${AMUSE_ENGINE_BINARIES_PATH}"
			)
		endforeach()
	endif()

endfunction()

#------------------------------------------------------------------------------
# ライブラリモジュールを追加する
# 使用方法はEngine/Source/Runtime/Template/CMakeLists.txtを参照してください
#------------------------------------------------------------------------------
function(amuse_add_library)

	# ソースコードを登録
	_amuse_glob_sources(sources)
	add_library(${PROJECT_NAME} STATIC ${sources})

	# API定義
	include(GenerateExportHeader)
	string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
	generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_API EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Export.h)
	target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    
    # 共通セットアップ
    _amuse_module_setup()

endfunction()

#------------------------------------------------------------------------------
# ライブラリモジュールを追加する
# 使用方法はEngine/Source/Runtime/Template/CMakeLists.txtを参照してください
#------------------------------------------------------------------------------
function(amuse_add_static_library)

	# ソースコードを登録
	_amuse_glob_sources(sources)
	add_library(${PROJECT_NAME} STATIC ${sources})
    
    # 共通セットアップ
    _amuse_module_setup()

endfunction()

#------------------------------------------------------------------------------
# 実行ファイルを追加する
# 使用方法はEngine/Source/Runtime/Template/CMakeLists.txtを参照してください
#------------------------------------------------------------------------------
function(amuse_add_executable)

	# ソースコードを登録
	_amuse_glob_sources(sources)
	add_executable(${PROJECT_NAME} ${sources})
    
    # 共通セットアップ
    _amuse_module_setup()

endfunction()

#------------------------------------------------------------------------------
# テストモジュールを追加する
# 使用方法はEngine/Source/Runtime/Template/CMakeLists.txtを参照してください
#------------------------------------------------------------------------------
function(amuse_add_test)
	if(AMUSE_TEST)

        # GoogleTestをリンク
        list(APPEND private_libs GTest::gtest_main)
        
        # ソースコードを登録
        _amuse_glob_sources(sources)
        add_executable(${PROJECT_NAME} ${sources})
        
        # 共通セットアップ
        _amuse_module_setup()

        # テストを登録
        gtest_discover_tests(${PROJECT_NAME})
		
	endif()
endfunction()
