//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/String/String.h>
#include <Amuse/Core/Utility/Noncopyable.h>

namespace Amuse {

	//! @brief スレッド生成時に指定する実行優先度。
	//! @ingroup AmuseCore
	enum class ThreadPriority : u32 {
		Level1,	//!< Critical
		Level2,	//!< Highest
		Level3,	//!< AboveNormal
		Level4,	//!< Normal
		Level5,	//!< BelowNormal
		Level6,	//!< Lowest
	};

	//! @brief スレッドの実行条件を指定する設定。
	//! @ingroup AmuseCore
	struct ThreadDesc {
		u32 affinity = 0xFFFFFFFF; //!< スレッドアフィニティ
		ThreadPriority priority = ThreadPriority::Level3; //!< スレッド優先度
	public:
		ThreadDesc() = default;
		//! @brief 優先度だけを指定してスレッド設定を初期化する。
		ThreadDesc(ThreadPriority p) :priority(p){}
	};

	//! @brief  スレッド
	//! @ingroup AmuseCore
	class Thread : Noncopyable {
	public:

		//! @brief				デフォルトコンストラクタ
		Thread();

		//! @brief				名前と実行関数を指定してスレッドを開始する
		//! 
		//! @param name			スレッド名
		//! @param entryPoint	新しいスレッド上で呼び出す関数オブジェクト
		Thread(StringView name, const Func<void()>& entryPoint)
			: Thread(name, ThreadDesc{},entryPoint){}

		//! @brief				名前、優先度、実行関数を指定してスレッドを開始する
		//! 
		//! @param name			スレッド名
		//! @param priority		スレッドの実行優先度
		//! @param entryPoint	新しいスレッド上で呼び出す関数オブジェクト
		Thread(StringView name, ThreadPriority priority, const Func<void()>& entryPoint)
			: Thread(name, ThreadDesc(priority), entryPoint) {}

		//! @brief				名前、詳細設定、実行関数を指定してスレッドを開始する
		//! 
		//! @param name			スレッド名
		//! @param desc			スレッドの実行設定
		//! @param entryPoint	新しいスレッド上で呼び出す関数オブジェクト
		Thread(StringView name, const ThreadDesc& desc, const Func<void()>& entryPoint);

		//! @brief				デストラクタ
		//! 
		//! @details			スレッドが終了するまで待機します。
		~Thread();

		//! @brief				スレッドが終了するまで待機する
		void join();

	public:

		//! @brief				現在のスレッドの実行権を他スレッドへ譲る
		static void YieldThread();

		//! @brief				現在のスレッドを指定ミリ秒だけ休止する
		static void Sleep(u32 milliSeconds);

		//! @brief				現在のスレッドIDを取得
		static u32 GetCurrentThreadId();

		//! @brief				スレッドを実行しているCPUのコア番号を取得
		static u32 GetCurrentCpuCore();


	private:
		void setup();
	private:

		Pimpl<class ThreadImpl> m_impl;

	};

}
