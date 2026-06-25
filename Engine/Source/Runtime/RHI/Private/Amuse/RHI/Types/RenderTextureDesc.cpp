//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <magic_enum.hpp>
#include <Amuse/RHI/Types/RenderTextureDesc.h>
#include <Amuse/RHI/RenderTexture.h>

namespace Amuse {
    bool RenderTextureDesc::isValid() const {

        // サイズ
        if (!(0 < size.width && 0 < size.height && 1 == size.depth)) {
            LOG_ERROR("TextureDescのサイズが不正です。[name={},size={}]", name, size);
            return false;
        }

        // フォーマット
        if (!RenderTexture::Supports(format)) {
            LOG_ERROR("サポート外のフォーマットです [name={}, type={}]", name,magic_enum::enum_name(format));
            return false;
        }

        return true;
    }

}
