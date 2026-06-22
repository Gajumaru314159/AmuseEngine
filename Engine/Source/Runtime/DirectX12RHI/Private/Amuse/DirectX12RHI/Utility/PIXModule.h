//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#ifdef AMUSE_DEBUG

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  説明
    class PIXModule {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //! @brief  説明
        PIXModule();
        ~PIXModule();

        void release();

    private:

        HMODULE m_hModule;


    };

}

#endif// AMUSE_DEBUG