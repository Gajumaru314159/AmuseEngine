//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/SamplerDesc.h>
#include <Amuse/RHI/Types/BindlessHandle.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      サンプラー
    class Sampler :public GraphicObject {
    public:
        //! @brief Default を実行する
        static Ref<Sampler> Default();
    public:

        //! @brief インスタンスを生成する
        static Ref<Sampler> Create(const SamplerDesc& desc);

    public:

        //! @brief      BindlessHandleを取得
        virtual BindlessHandle getHandle()const = 0;

    };

}

