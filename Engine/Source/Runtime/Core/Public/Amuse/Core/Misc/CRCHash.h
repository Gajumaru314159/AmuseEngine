//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

    //! @brief ハッシュ値の計算を行うクラス
    //! @ingroup AmuseCore
    class CRCHash {
    public:

        //! @brief 指定した文字列のハッシュ値を求める
        //! 
        //! @param str 元文字配列
        //! @param len 配列の長さ
        //! @return ハッシュ値
        static u32 Compute(const char* str, s32 len = -1);


        //! @brief 指定した文字列のハッシュ値を求める
        //! 
        //! @param str 元文字配列
        //! @param len 配列の長さ
        //! @return ハッシュ値
        static u32 Compute(const wchar_t* str, s32 len = -1);


        //! @brief 指定した文字列のハッシュ値を求める
        //! 
        //! @param str 元文字配列
        //! @param len 配列の長さ
        //! @return ハッシュ値
        static u32 Compute(const char16_t* str, s32 len = -1);


        //! @brief 指定した文字列のハッシュ値を求める
        //! 
        //! @param str 元文字配列
        //! @param len 配列の長さ
        //! @return ハッシュ値
        static u32 Compute(const char32_t* str, s32 len = -1);

    };

    //! @cond
    namespace Internal {
        //! @ingroup AmuseCore
        struct CRCHashInternal {
            static const unsigned* GetCRCTable();
        };

    } 
    //! @endcond

} 
