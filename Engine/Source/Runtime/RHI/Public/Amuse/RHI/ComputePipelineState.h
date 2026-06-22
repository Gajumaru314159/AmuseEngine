//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/ComputePipelineStateDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      コンピュートパイプラインステート
    class ComputePipelineState :public GraphicObject {
    public:

        //! @brief  生成
        static Ref<ComputePipelineState> Create(const ComputePipelineStateDesc& desc);

    };

}