//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Template/include.h>
#include <Amuse/Core/Utility/Ref.h>
#include <Amuse/Core/Job/JobHandle.h>

namespace Amuse::Core {

    class JobExecutor;
    class Job;
    class JobSystem;

    //! @brief		ジョブ・グループ
    //! @details    JobGroupは実行依存関係を持ちます。
    //!             同期ポイントはEntryとLeaveがあります。
    class JobGroup {
        friend class JobSystem;
    public:

        //! @brief コンストラクタ
        JobGroup(JobSystem& sys, StringView name);

        //! @brief デストラクタ
        ~JobGroup();

        //! @brief 名前を取得
        auto getName()const->const String&;

        //! @brief サブ JobGroup を作成
        auto createSub(StringView name) -> Ref<JobGroup>;
        //! @brief このグループの子 JobGroup を作成する。
        auto createChild(StringView name) -> Ref<JobGroup>;
        //! @brief 既存の JobGroup を子グループとして登録する。
        void addChild(Ref<JobGroup>& group);

        //! @brief このグループにジョブを追加し、寿命管理用ハンドルを返す。
        auto addJob(StringView name, Action&& action)-> JobHandle&;


    public:

        //! @brief グループ内のジョブを executor へ投入する。
        void execute(JobExecutor& executor);
        //! @brief 追加予約と解放予約を反映してグループ状態を更新する。
        void update(JobExecutor& executor);

        //! @brief 指定したジョブを解放予約へ移す。
        void removeJob(Job&);

        //! @brief このグループの解放をリクエストする。
        void requestRelease();

    private:

        void onLeaveSubGroup(JobExecutor& executor);

        s32 countLeafGroup()const;

    private:

        String                  m_name;
        JobSystem& m_system;

        // 所有
        Vector<Job*>        m_jobs;
        Vector<JobGroup*>   m_subGroups;
        Vector<JobGroup*>   m_childGroups;

        // 追加予約
        SpinLock                m_entryLock;
        Vector<UPtr<JobGroup>>   m_entrySubGroups;
        Vector<UPtr<JobGroup>>   m_entryChildGroups;
        Vector<UPtr<Job>>        m_entryJobs;

        // 解放予約
        HashSet<Job*>           m_leaveJobs;
        HashSet<JobGroup*>      m_leaveGroups;


        // 実依存
        Vector<JobGroup*>        m_deps;



        Atomic<s32>             m_jobIndex;
        Atomic<s32>             m_childIndex;
        Atomic<s32>             m_executedNum;

        Atomic<s32>             m_dependParentCount;
        Atomic<s32>             m_dependSubCount;
    };

}
