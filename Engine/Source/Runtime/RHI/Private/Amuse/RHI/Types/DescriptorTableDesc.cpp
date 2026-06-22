//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Types/DescriptorTableDesc.h>
#include <Amuse/RHI/DescriptorLayout.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	DescriptorTableDesc::DescriptorTableDesc(const Ref<DescriptorLayout>& layout) : layout(layout) {}

}