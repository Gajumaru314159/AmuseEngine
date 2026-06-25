//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
/*
#include <Amuse/Core/Job/Job.h>
#include <Amuse/Core/Job/JobGroup.h>

namespace Amuse {

    JAmuse::Job(JobGroup& group, StringView name,Action&& action)
        : m_group(group)
        , m_name(name)
        , m_action(action)
        , m_isSuspended(true)
    {

    }

    //! @brief      デストラクタ
    //! @details    デストラクタは必ずフレームの最初に行われる
    JAmuse::~Job() {
        // 削除順は JobHandle::~JobHandle → JAmuse::requestRelease / JAmuse::~Job
        m_group.removeJob(*this);
    }

    auto JAmuse::getName()const->const String& {
        return m_name;
    }

    void JAmuse::execute() {
        if (m_action) {
            m_action();
        }
    }
    void JAmuse::setSuspend(bool value) {
        m_isSuspended = value;
    }
    bool JAmuse::isSuspended()const {
        return m_isSuspended;
    }

    void JAmuse::requestRelease() {
        setSuspend(true);
        m_group.removeJob(*this);
    }

}
*/