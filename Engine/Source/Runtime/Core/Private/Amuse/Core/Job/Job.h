//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Template/include.h>
#include <Amuse/Core/Utility/Ref.h>
#include <Amuse/Core/Job/JobHandle.h>

namespace Amuse {

    class JobGroup;

    //! @brief Job
    //! @ingroup AmuseCore
    class Job {
    public:
        Job(JobGroup&,StringView name,Action&& action);
        ~Job();

        auto getName()const->const String&;

        void execute();

        void setSuspend(bool);
        bool isSuspended()const;

        void requestRelease();

    private:
        JobGroup&       m_group;
        String          m_name;
        Atomic<bool>    m_isSuspended;
        Func<void()>    m_action;
    };

}