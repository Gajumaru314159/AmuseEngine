//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse::Input {
    using namespace Amuse::Core;

    //! @brief  コンフィグ
    struct InputConfig {
        bool useKeyboard = true; //!< キーボード使用フラグ
        bool useMouse = true; //!< マウス使用フラグ
    };

}
