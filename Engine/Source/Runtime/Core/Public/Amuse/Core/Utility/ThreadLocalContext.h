//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Utility/Nonmovable.h>
#include <Amuse/Core/Thread/Thread.h>
namespace Amuse::Core {

    template<class T>
    class ThreadLocalContext : Nonmovable {
    public:
        T& operator*()
        {
            return *operator->();
        }

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

        void forEach(Func<void(T&)> visitor)
        {
            if (!visitor) return;
            ScopeLock lock(m_lock);
            for (auto& [threadId,storage] : m_storages)
            {
                visitor(*storage.get());
            }
        }

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