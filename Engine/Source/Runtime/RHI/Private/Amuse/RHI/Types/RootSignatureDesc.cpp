//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Types/RootSignatureDesc.h>
#include <Amuse/RHI/DescriptorLayout.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	RootSignatureDesc::RootSignatureDesc(
		std::initializer_list<Ref<DescriptorLayout>> layouts,
		std::initializer_list<StaticSamplerDesc> samplers,
		const RootConstantsDesc& constants,
		RootSignatureFlags flags)
		: layouts(layouts)
		, samplers(samplers)
		, constants(constants)
		, flags(flags) {}

}