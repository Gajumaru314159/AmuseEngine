//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/BufferDesc.h>

namespace Amuse::RPI {

    //! @brief      FrameGraph中間テクスチャ
    class FGBuffer{
    public:
        using Desc = RHI::BufferDesc;

        void create(const Desc& desc, void* allocator);
        void destroy(const Desc& desc, void* allocator);
        static std::string toString(const Desc& desc);

        void preRead(const Desc& desc, uint32_t flags, void* ctx);
        void preWrite(const Desc& desc, uint32_t flags, void* ctx);

        Ref<RHI::Buffer> instance;
    };

}