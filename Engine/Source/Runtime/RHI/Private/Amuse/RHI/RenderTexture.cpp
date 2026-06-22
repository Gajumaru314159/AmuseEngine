//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief          生成
    Ref<RenderTexture> RenderTexture::Create(const RenderTextureDesc& desc) {
        if (auto device = Device::Get()) {
            return device->createRenderTexture(desc);;
        }
        return nullptr;
    }

    bool RenderTexture::Supports(TextureFormat format) {
        if (auto device = Device::Get()) {
            return device->supportsForRenderTexture(format);
        }
        return false;
    }

}