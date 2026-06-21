#------------------------------------------------------------------------------
# コンパイラーごとの設定
#------------------------------------------------------------------------------

if(MSVC)
    # 警告設定
    add_definitions(/W4)            # 警告レベルを最大にする
    add_compile_options(/wd26495)   # メンバの未初期化
    add_compile_options(/wd26451)   # 算術オーバーフロー
    add_compile_options(/wd4458)    # 'identifier' を宣言すると、クラス メンバーが隠蔽されます
    add_compile_options(/wd4456)    # 'identifier' を宣言すると、以前のローカル宣言が隠蔽されます
    add_compile_options(/wd4201)    # 非標準の拡張機能が使用されています: 無名の構造体または共用体です
    add_compile_options(/wd4101)    # 'identifier': 参照されていないローカル変数
    add_compile_options(/wd4100)    # 仮パラメーターが関数の本体で参照されていません。 未参照のパラメーターは無視されます。
    add_compile_options(/wd4267)    # 'var': 'size_t' から 'type' への変換です。データが失われる可能性があります。
    add_compile_options(/wd4244)    # 'argument' : 'type1' から 'type2' への変換です。データが失われる可能性があります
    add_compile_options(/wd4702)    # unreachable code

    # 文字コード設定
    add_compile_options("/utf-8")   # UTF-8を強制する
    add_definitions(-D_UNICODE)     # 文字リテラルをユニコードとする
    add_definitions(-DUNICODE)      # 文字リテラルをユニコードとする

    # __cplusplus有効化
    add_definitions(/Zc:__cplusplus)# __cplusplusを使えるようにする

endif()

if(CLANG OR GCC)
    # 警告設定
    add_compile_options(-Wno-everything -Werror -Wno-invalid-constexpr)

    # 文字コード設定
    add_compile_options(-utf-8)

    # NDEBUG設定
    add_compile_options(-DNDEBUG)

endif()