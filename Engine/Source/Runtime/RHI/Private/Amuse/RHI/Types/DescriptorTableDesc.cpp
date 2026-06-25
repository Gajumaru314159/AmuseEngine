//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Types/DescriptorTableDesc.h>
#include <Amuse/RHI/DescriptorLayout.h>

namespace Amuse {
	DescriptorTableDesc::DescriptorTableDesc(const Ref<DescriptorLayout>& layout) : layout(layout) {}

}