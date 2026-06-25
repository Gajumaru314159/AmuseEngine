//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Platform/Type/Language.h>

namespace Amuse {
    //! @brief      システムをServiceInjectorに登録
    void RegisterPlatformService(ServiceInjector&);

    //! @brief  システム情報の取得
    namespace System {

        //! @brief  プラットフォームの言語を取得
        Language GetLanguage();

        //! @brief  プラットフォーム設定
        bool Setup();

        //! @brief  プラットフォーム更新
        bool Update();

    };

}