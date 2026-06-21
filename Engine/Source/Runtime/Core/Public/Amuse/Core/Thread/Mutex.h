//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Utility/Noncopyable.h>
#include <Amuse/Core/Utility/Pimpl.h>

namespace Amuse::Core {

    //! @brief  ミューテックス
    class Mutex : Noncopyable{
    public:

        //! @brief  コンストラクタ
        Mutex();


        //! @brief  デストラクタ
        ~Mutex();


        //! @brief  ロックを取得する
        void lock();


        //! @brief  ロックの取得を試みる
        bool try_lock();


        //! @brief  ロックを手放す
        void unlock();

    private:

        Pimpl<class MutexImpl> m_impl;

    };

}