//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/DescriptorTableDesc.h>

namespace Amuse {
    //! @brief      デスクリプタテーブル
    //! @ingroup AmuseRHI
    class DescriptorTable :public GraphicObject {
    public:

        //! @brief              コンストラクタ
        static Ref<DescriptorTable> Create(const DescriptorTableDesc& desc);

    public:

        //! @brief getDesc を実行する
        virtual const DescriptorTableDesc& getDesc() const = 0;

        //! @brief  リソースを設定
        //! @{
        virtual bool setResource(s32 index, const Ref<Buffer>& resource) = 0;
        //! @brief setResource を実行する
        virtual bool setResource(s32 index, const Ref<Texture>& resource) = 0;
        //! @brief setResource を実行する
        virtual bool setResource(s32 index, const Ref<Sampler>& resource) = 0;
        //! @}

    };

}
