//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse {

    //! @brief		クラスコピーの禁止
    //! 
    //! @details	このクラスをprivate継承すると外部からのコピーコンストラクタと代入演
    //!				算子の呼び出しが禁止される。
    //! @ingroup AmuseCore
    class Noncopyable {
    protected:
        //! @cond
        Noncopyable() {}
        ~Noncopyable() {}
    private:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
        //! @endcond
    };

}
