//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/ComputePipelineStateDesc.h>

namespace Amuse {
    //! @brief      コンピュートパイプラインステート
    //! @ingroup AmuseRHI
    class ComputePipelineState :public GraphicObject {
    public:

        //! @brief  生成
        static Ref<ComputePipelineState> Create(const ComputePipelineStateDesc& desc);

    };

}