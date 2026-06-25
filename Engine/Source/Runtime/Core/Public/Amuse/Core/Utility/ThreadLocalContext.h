//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Utility/Nonmovable.h>
#include <Amuse/Core/Thread/Thread.h>
namespace Amuse {

    template<class T>
    //! @brief スレッドごとのコンテキストを表す。
    //! @ingroup AmuseCore
    class ThreadLocalContext : Nonmovable {
    public:
        //! @brief 参照先または演算結果を取得する。
        T& operator*()
        {
            return *operator->();
        }

        //! @brief 参照先メンバーへアクセスする。
        T* operator->()
        {
            thread_local T* pStorage = nullptr;

            if (pStorage==nullptr)
            {
                ScopeLock lock(m_lock);
                auto& storage = m_storages[Thread::GetCurrentThreadId()] = std::make_unique<T>();
                pStorage = storage.get();
            }

            return pStorage;
        }

        //! @brief 各スレッドに作成済みのコンテキストを走査する。
        void forEach(Func<void(T&)> visitor)
        {
            if (!visitor) return;
            ScopeLock lock(m_lock);
            for (auto& [threadId,storage] : m_storages)
            {
                visitor(*storage.get());
            }
        }

        //! @brief 各スレッドに作成済みのコンテキストを読み取り専用で走査する。
        void forEach(Func<void(const T&)> visitor) const
        {
            if (!visitor) return;
            ScopeLock lock(m_lock);
            for (auto& [threadId,storage] : m_storages)
            {
                visitor(*storage.get());
            }
        }
    private:
        using ThreadId = u32;
        mutable SpinLock m_lock;
        HashMap<ThreadId,UPtr<T>> m_storages;
    };

}
