//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse::Core {

    //! @brief  圧縮
    class Compression {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //! @brief 圧縮処理の共通インターフェース用クラス。

    private:



    };

    //! @brief GDeflate 圧縮ユーティリティを表す。
    class GDeflate {
    public:
        static constexpr u32 MinCompressionLevel = 1; //!< GDeflate で指定できる最小圧縮レベル
        static constexpr u32 MaxCompressionLevel = 12; //!< GDeflate で指定できる最大圧縮レベル
    public:
        //! @brief 入力バッファを GDeflate 形式で圧縮する。
        static bool Compress(u8* output,size_t* outputSize,const u8* in,size_t inSize,u32 level,u32 flags);
        //! @brief GDeflate 形式の入力バッファを展開する。
        static bool Decompress(u8* output, size_t outputSize, const u8* in, size_t inSize, u32 numWorkers);
    };






    //===============================================================
    // インライン関数
    //===============================================================
    //! @cond



    //! @endcond
}
