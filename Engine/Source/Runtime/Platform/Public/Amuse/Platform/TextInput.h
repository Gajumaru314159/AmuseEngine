//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;


    enum class TextInputMode {
        DenyCtrl = get_bit(0),
        AllowEnter = get_bit(1),
        AllowTab = get_bit(2),
        AllowBackSpace = get_bit(3),
        AllowDelete = get_bit(4),

        Default = AllowEnter | AllowTab | AllowBackSpace | AllowDelete,
    };

    //! @brief  説明
    class TextInput {
    public:

        //! @brief 確定済みテキストを取得する
        static String GetText();
        //! @brief 入力中テキストを更新する
        static size_t UpdateText(StringView text, size_t cursor, TextInputMode mode = TextInputMode::Default);
        //! @brief 入力中テキストを更新する
        static size_t UpdateText(StringView text, TextInputMode mode = TextInputMode::Default);
        //! @brief 編集中テキストを取得する
        static String GetEditingText();


        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //! @brief  説明

    private:



    };

}
