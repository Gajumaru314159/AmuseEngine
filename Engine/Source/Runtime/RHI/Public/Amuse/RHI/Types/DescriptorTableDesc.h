//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      スワップチェーン定義
    struct DescriptorTableDesc {
        String name; //!< 名前
		Ref<DescriptorLayout> layout; //!< layout

		DescriptorTableDesc() = default;
        //! @brief DescriptorTableDesc を実行する
        DescriptorTableDesc(const Ref<DescriptorLayout>& layout);
    };

}
