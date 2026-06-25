//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Profile/Profile.h>
#include <Amuse/Core/Utility/Noncopyable.h>

#if AMUSE_DEBUG_SPIN_LOCK
#	define AMUSE_DEBUG_SPIN_LOCK_CONTEX(context)	context
#else
#	define AMUSE_DEBUG_SPIN_LOCK_CONTEX(contex)	/**/
#endif

namespace Amuse {

    //! @brief  スピンロック
    //! @ingroup AmuseCore
    class SpinLock : Noncopyable {
    public:

        //! @brief  コンストラクタ
        SpinLock();


        //! @brief  デストラクタ
        ~SpinLock();


        //! @brief  ロックを取得する
        void lock();


        //! @brief  ロックを手放す
        void unlock();

    private:

        //! @ingroup AmuseCore
        struct Impl {
            std::atomic<bool> m_lock;
            AMUSE_DEBUG_SPIN_LOCK_CONTEX(
                std::atomic<unsigned int> m_threadId{ 0 };
            )
            void lock();
            void unlock();
        };

        AMUSE_PROFILE_LOCK(Impl,m_impl);

    };

}