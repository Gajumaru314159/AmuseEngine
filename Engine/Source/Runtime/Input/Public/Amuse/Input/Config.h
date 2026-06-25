//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
    //! @brief  コンフィグ
    //! @ingroup AmuseInput
    struct InputConfig {
        bool useKeyboard = true; //!< キーボード使用フラグ
        bool useMouse = true; //!< マウス使用フラグ
    };

}
