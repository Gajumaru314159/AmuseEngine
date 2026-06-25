//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Thread/Mutex.h>
#include <mutex>
#include <atomic>
#include <Amuse/Core/Thread/Thread.h>
#include <Amuse/Core/Profile/Profile.h>

#if AMUSE_DEBUG_MUTEX
#	define AMUSE_DEBUG_MUTEX_CONTEX(context)	context
#else
#	define AMUSE_DEBUG_MUTEX_CONTEX(contex)	/**/
#endif

namespace Amuse {

	//! @brief  説明
	class MutexImpl {
	public:
		AMUSE_PROFILE_LOCK(std::mutex, m_mutex);
		AMUSE_DEBUG_MUTEX_CONTEX(
			std::atomic<u32> m_threadId{ 0 };
		)
	};

	//! @brief  コンストラクタ
	Mutex::Mutex() {

	}


	//! @brief  デストラクタ
	Mutex::~Mutex() {

	}


	//! @brief  ロックを取得する
	void Mutex::lock() {

		AMUSE_DEBUG_MUTEX_CONTEX(
			u32 id = m_impl->m_threadId.load();
			assert(id != Thread::GetCurrentThreadId());
		)

		m_impl->m_mutex.lock();

		AMUSE_DEBUG_MUTEX_CONTEX(
			m_impl->m_threadId.store(Thread::GetCurrentThreadId());
		)
	}


	//! @brief  ロックの取得を試みる
	bool Mutex::try_lock() {
		return m_impl->m_mutex.try_lock();
	}


	//! @brief  ロックを手放す
	void Mutex::unlock() {
		AMUSE_DEBUG_MUTEX_CONTEX(
			m_impl->m_threadId.store(0);
		)
		m_impl->m_mutex.unlock();
	}

}

#undef AMUSE_DEBUG_MUTEX_CONTEX