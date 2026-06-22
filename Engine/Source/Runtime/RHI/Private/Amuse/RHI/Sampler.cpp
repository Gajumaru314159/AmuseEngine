//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Sampler.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/SystemResource.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	static Ref<Sampler> GetPreset(PresetSampler type) {
		if (auto instance = SystemResource::Get()) {
			return instance->getPresetSampler(type);
		}
		return nullptr;
	}

	Ref<Sampler> Sampler::Default() {
		return GetPreset(PresetSampler::Default);
	}

    Ref<Sampler> Sampler::Create(const SamplerDesc& desc) {
		if (auto device = Device::Get()) {
			return device->createSampler(desc);
		}
		return nullptr;
    }

}