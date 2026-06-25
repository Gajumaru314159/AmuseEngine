//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Utility/Noncopyable.h>

namespace Amuse {

	//! @brief		スコープ単位でのロックユーティリティ
	//! 
	//! @details	ロックするクラスは lock() と unlock() を持つ必要があります。
	//!				コンストラクタでロックし、スコープを抜けったときに自動的に解除します。
	//! @ingroup AmuseCore
	template<class TLockable>
	class ScopeLock : Noncopyable {
	public:

		//! @brief  コンストラクタ
		ScopeLock(TLockable& lockable)
			:m_lockable(lockable)
		{
			m_lockable.lock();
		}


		//! @brief  デストラクタ
		~ScopeLock() {
			m_lockable.unlock();
		}

	private:

		TLockable& m_lockable;

	};

}
