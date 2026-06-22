//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <magic_enum.hpp>
#include <Amuse/RHI/Types/TextureDesc.h>
#include <Amuse/RHI/Texture.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    bool TextureDesc::isValid() const {

        // サイズ
        bool isValidSize = true;

        if (type == TextureType::Texture1D) {
            isValidSize &= 0 < size.width && 1 == size.height && 1 == size.depth;
        }
        if (type == TextureType::Texture2D) {
            isValidSize &= 0 < size.width && 0 < size.height && 1 == size.depth;
        }
        if (type == TextureType::Texture3D) {
            isValidSize &= 0 < size.width && 0 < size.height && 0 < size.depth;
        }
        if (type == TextureType::Cube) {
            isValidSize &= 0 < size.width && 0 < size.height && 1 == size.depth;
        }
        if (!isValidSize) {
            LOG_ERROR("TextureDescのサイズが不正です。[name={},size={}]", name, size);
            return false;
        }

        // 配列
        if (type == TextureType::Texture3D && 0 < arrayNum) {
            LOG_ERROR("Texture3Dは配列に対応していません [name={}]", name);
            return false;
        }

        // フォーマット
        if (!Texture::Supports(format, type)) {
            LOG_ERROR("サポート外のフォーマットです [name={}, format={}, type={}]", name,magic_enum::enum_name(format),magic_enum::enum_name(type));
            return false;
        }

        return true;
    }

}
