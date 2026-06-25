//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/DescriptorTable.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse {
    Ref<DescriptorTable> DescriptorTable::Create(const DescriptorTableDesc& desc) {
        if (auto device = Device::Get()) {
            return device->createDescriptorTable(desc);
        }
        return nullptr;
    }

}