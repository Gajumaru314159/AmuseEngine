//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/RootSignatureDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief      ルートシグネチャ
    class RootSignature :public GraphicObject {
    public:
        
        //! @brief  生成
        static Ref<RootSignature> Create(const RootSignatureDesc& desc);

    public:

        //! @brief      定義を取得
        virtual const RootSignatureDesc& getDesc()const = 0;

    };

}