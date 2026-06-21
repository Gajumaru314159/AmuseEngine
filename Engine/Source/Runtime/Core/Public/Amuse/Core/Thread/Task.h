//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/String/String.h>
#include <Amuse/Core/Template/Utility/Function.h>

namespace Amuse::Core {

	class Task {
	public:
		//! @brief インスタンスを生成する。
		static Task Create(Action action) {

		}
		//! @brief アクションをタスクとして開始する。
		static Task Run(Action action) {
		}

		//! @brief インデックス付きアクションを並列タスクとして開始する。
		static Task Parallel(Func<void(s32)> action) {
		}

		//! @brief 複数タスクの完了を待機する。
		static void WaitAll(Task& task1, Task& task2) {
		}

		// ContinueWith
		// Wait
		// WaitAny
		// 
	public:
		//! @brief 完了済みか判定する。
		bool isCompleted()const {
		}
		//! @brief 完了を待機する。
		void wait();
	};


	void TaskSample() {

		Task::Run([]() {});
		
		auto task1 = Task::Create([]() {});
		auto task2 = Task::Create([]() {});

		Task::WaitAll(task1,task2);



		// Animation更新
		Task::Parallel(
			[](s32 index) {
				// 並列処理
			}
		);

		// Physics更新
		Task::Parallel(
			[](s32 index) {
				// 並列処理
			}
		);

		// Transform更新
		Task::Parallel(
			[](s32 index) {
				// 並列処理
			}
		);

		// VFX更新
		Task::Parallel(
			[](s32 index) {
				// 並列処理
			}
		);

		// UI更新
		Task::Parallel(
			[](s32 index) {
				// 並列処理
			}
		);

		for (s32 i = 0; i < 16; ++i) {
			tf.emplace([i] { });
		}


	}


}
