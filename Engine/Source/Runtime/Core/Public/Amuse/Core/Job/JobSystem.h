//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/String/StringView.h>
#include <Amuse/Core/Utility/Singleton.h>
#include <Amuse/Core/Utility/Pimpl.h>

namespace Amuse {

	class JobGroup;
	class Job;

	//! @brief ジョブグループとジョブの生成、実行、待機を管理する。
	//! @ingroup AmuseCore
	class JobSystem : public Singleton<JobSystem> {
	public:

		//! @brief ジョブシステムを初期化する。
		JobSystem();

		//! @brief 登録済みジョブの実行を開始する。
		void execute();

		//! @brief 実行中ジョブの完了を待機する。
		void wait();

		//! @brief 指定名の JobGroup を作成する。
		auto createJobGroup(StringView name)->JobGroup&;

		//! @brief 指定グループに属する Job を作成する。
		auto createJob(StringView name, JobGroup& group, Action&& action);

	private:
		Pimpl<class JobSystemImpl> m_impl;
	};

}
