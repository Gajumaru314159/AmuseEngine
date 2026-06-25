//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/DescriptorLayout.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse {
    Ref<DescriptorLayout> DescriptorLayout::Create(const DescriptorLayoutDesc& desc) {
        if (auto device = Device::Get()) {
            return device->createDescriptorLayout(desc);
        }
        return nullptr;
    }

}