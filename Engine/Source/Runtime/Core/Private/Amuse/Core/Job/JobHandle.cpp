//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
/*
#include <Amuse/Core/Job/JobHandle.h>
#include <Amuse/Core/Job/Job.h>
#include <Amuse/Core/Job/JobExecutor.h>

namespace Amuse::Core {

    //! @brief コンストラクタ
    JobHandle::JobHandle() {

    }

    //! @brief コンストラクタ
    JobHandle::JobHandle(Job& job) {
        m_job = &job;
    }

    //! @brief デストラクタ
    JobHandle::~JobHandle() {
        release();
    }

    //! @brief 解放
    void JobHandle::release() {
        if (m_job) {
            m_job->requestRelease();
        }
        m_job = nullptr;
    }


    //! @brief コンストラクタ
    JobGroupHandle::JobGroupHandle() {

    }

    //! @brief コンストラクタ
    JobGroupHandle::JobGroupHandle(JobGroup& group) {
        m_group = &group;
    }

    //! @brief デストラクタ
    JobGroupHandle::~JobGroupHandle() {
        release();
    }

    //! @brief 解放
    void JobGroupHandle::release() {
        if (m_group) {
            m_group->requestRelease();
        }
        m_group = nullptr;
    }

}
*/