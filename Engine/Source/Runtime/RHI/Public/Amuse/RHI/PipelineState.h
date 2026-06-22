//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      パイプラインステート
    class PipelineState :public GraphicObject {
    public:

        //! @brief  生成
        static Ref<PipelineState> Create(const PipelineStateDesc& desc);

    };

}