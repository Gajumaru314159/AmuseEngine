//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Types/RenderTextureDesc.h>

namespace Amuse {

    //! @brief      FrameGraph中間テクスチャ
    //! @ingroup AmuseRPI
    class FGTexture {
    public:
        using Desc = RenderTextureDesc;

        void create(const Desc& desc, void* allocator);
        void destroy(const Desc& desc, void* allocator);
        static std::string toString(const Desc& desc);

        void preRead(const Desc& desc, uint32_t flags, void* ctx);
        void preWrite(const Desc& desc, uint32_t flags, void* ctx);

        Ref<RenderTexture> instance;
    };

}