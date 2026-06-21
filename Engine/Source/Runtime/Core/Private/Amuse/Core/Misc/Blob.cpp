//***********************************************************
//! @file
//! @author Gajumaru
//***********************************************************
#include <Amuse/Core/Misc/Blob.h>
#include <Amuse/Core/File/Stream.h>

namespace Amuse::Core {

    //! @brief ストリームから生成
    Blob::Blob(Stream& stream) {

        if (stream.canRead()) {
            resize(stream.size());
            stream.read(data(), size());
        }

    }

} 