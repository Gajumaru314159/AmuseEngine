//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

namespace Amuse {

    //! @brief		クラスムーブの禁止
    //! 
    //! @details	このクラスをprivate継承すると外部からのムーブコンストラクタとムーブ
    //!				代入演算子の呼び出しが禁止される。
    //! @ingroup AmuseCore
    class Nonmovable {
    protected:
        //! @cond
        Nonmovable() {}
        ~Nonmovable() {}
    private:
        Nonmovable(Nonmovable&&) = delete;
        Nonmovable& operator=(Nonmovable&&) = delete;
        //! @endcond
    };

}