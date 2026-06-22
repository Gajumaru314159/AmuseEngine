//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/SwapChain.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  生成
    Ref<SwapChain> SwapChain::Create(const SwapChainDesc& desc) {
        if (auto device= Device::Get()) {
            return device->createSwapChain(desc);
        }
        return nullptr;
    }

}