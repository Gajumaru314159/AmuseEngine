//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
/*
#include <Amuse/Core/Job/Job.h>
#include <Amuse/Core/Job/JobGroup.h>

namespace Amuse::Core {

    JAmuse::Core::Job(JobGroup& group, StringView name,Action&& action)
        : m_group(group)
        , m_name(name)
        , m_action(action)
        , m_isSuspended(true)
    {

    }

    //! @brief      デストラクタ
    //! @details    デストラクタは必ずフレームの最初に行われる
    JAmuse::Core::~Job() {
        // 削除順は JobHandle::~JobHandle → JAmuse::Core::requestRelease / JAmuse::Core::~Job
        m_group.removeJob(*this);
    }

    auto JAmuse::Core::getName()const->const String& {
        return m_name;
    }

    void JAmuse::Core::execute() {
        if (m_action) {
            m_action();
        }
    }
    void JAmuse::Core::setSuspend(bool value) {
        m_isSuspended = value;
    }
    bool JAmuse::Core::isSuspended()const {
        return m_isSuspended;
    }

    void JAmuse::Core::requestRelease() {
        setSuspend(true);
        m_group.removeJob(*this);
    }

}
*/