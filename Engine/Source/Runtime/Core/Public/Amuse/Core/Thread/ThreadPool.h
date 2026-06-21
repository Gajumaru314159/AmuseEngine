//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Thread/Thread.h>
#include <Amuse/Core/Utility/Singleton.h>

namespace Amuse::Core {

	class ThreadPool : public Singleton<ThreadPool> {
	public:

		ThreadPool();
		~ThreadPool() override;

		void resize(s32 threadNum);
		void enqueue(Action&& action);

	private:

		void join();
		void process();

	private:
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::atomic_bool m_running = false;
		Deque<Action> m_queue;

		Vector<Pimpl<Thread>> m_threads;
	};

}