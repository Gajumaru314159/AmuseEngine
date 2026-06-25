//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>

namespace Amuse {

	class Job;
	class JobGroup;

	//! @brief		ジョブ・ハンドル
	//! @details	ジョブに登録した関数がオブジェクトの解放後に実行されないように
	//!				するためのハンドルです。JobHandleの解放と同時にJobはサスペンド
	//!				状態に移行します。
	//! @ingroup AmuseCore
	class JobHandle : Noncopyable, Nonmovable {
	public:
		JobHandle();
		//! @brief 指定ジョブの寿命監視ハンドルを初期化する。
		JobHandle(Job&);
		~JobHandle();
		//! @brief 所有権を解放する。
		void release();
	private:
		Job* m_job = nullptr;
	};

	//! @brief		ジョブ・グループ・ハンドル
	//! @details	ジョブに登録した関数がオブジェクトの解放後に実行されないように
	//!				するためのハンドルです。JobHandleの解放と同時にJobはサスペンド
	//!				状態に移行します。
	//! @ingroup AmuseCore
	class JobGroupHandle : Noncopyable, Nonmovable {
	public:
		JobGroupHandle();
		//! @brief 指定ジョブグループの寿命監視ハンドルを初期化する。
		JobGroupHandle(JobGroup&);
		~JobGroupHandle();
		//! @brief 所有権を解放する。
		void release();
	private:
		JobGroup* m_group = nullptr;
	};

}
