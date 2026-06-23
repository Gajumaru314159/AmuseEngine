//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/FrameGraph/FGBuffer.h>
#include <Amuse/RPI/FrameGraph/FGResourcePool.h>
#include <magic_enum.hpp>

namespace Amuse::RPI {

	void FGBuffer::create(const Desc& desc, void* allocator) {
		AMUSE_ASSERT_EXPR(allocator);
		instance = static_cast<FGResourcePool*>(allocator)->createBuffer(desc);
	}
	void FGBuffer::destroy(const Desc& desc, void* allocator) {
		AMUSE_ASSERT_EXPR(allocator);
		AMUSE_ASSERT_EXPR(instance);
		static_cast<FGResourcePool*>(allocator)->destroyBuffer(desc, instance);
	}
	std::string FGBuffer::toString(const Desc& desc) {
		auto str = Format(
			"Name  :{}\n"
			"Type  :{}\n"
			"Usaege:{}\n"
			"Size  :{}\n"
			"Stride:{}",
			desc.name,
			magic_enum::enum_name(desc.state),
			magic_enum::enum_name(desc.state),
			desc.size,
			desc.stride
		);
		return std::move(str);
	}

	void FGBuffer::preRead([[maybe_unused]]const Desc& desc, [[maybe_unused]]uint32_t flags, [[maybe_unused]]void* ctx) {
		// auto& cmd = *static_cast<Ref<RHI::CommandList>*>(ctx);
	}
	void FGBuffer::preWrite([[maybe_unused]]const Desc& desc, [[maybe_unused]]uint32_t flags, [[maybe_unused]]void* ctx) {
		// auto& cmd = *static_cast<Ref<RHI::CommandList>*>(ctx);
	}

}