//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/RHI/All.h>
#include <Amuse/VulkanRHI/System.h>
#include <Amuse/VulkanRHI/VulkanRHIConfig.h>
#include <Amuse/Platform/System.h>
#include <magic_enum.hpp>
#ifdef OS_WINDOWS
#include <Amuse/DirectX12RHI/System.h>
#include <Amuse/DirectX12RHI/DirectX12RHIConfig.h>
#endif
using namespace Amuse::Core;
using namespace Amuse::RHI;
using namespace Amuse::Platform;

struct DirectX12Tag {};
struct VulkanTag {};

template<typename T>
class RHITest : public ::testing::Test {
protected:
	virtual void SetUp() {

		ServiceInjector injector;
		ServiceContainer container;

#ifdef OS_WINDOWS
		if constexpr (std::is_same_v<T, DirectX12Tag>) {
			Amuse::RHI::RegisterDirectX12RHIService(injector);
		}
#endif
		if constexpr (std::is_same_v<T, VulkanTag>) {
			Amuse::RHI::RegisterVulkanRHIService(injector);
		}

		Amuse::RHI::RegisterRHIService(injector);

#ifdef OS_WINDOWS
		m_dx12config.enableDebugLayer = true;
		m_dx12config.breakWithWarning = true;
#endif
		m_vkconfig.enableDebugLayer = true;
		injector.bind(m_config);
#ifdef OS_WINDOWS
		injector.bind(m_dx12config);
#endif
		injector.bind(m_vkconfig);

		injector.create<Device>(m_container);

	};
	virtual void TearDown() {};

	//Logger m_logger;
	RHIConfig m_config;
#ifdef OS_WINDOWS
	DirectX12RHIConfig m_dx12config;
#endif
	VulkanRHIConfig m_vkconfig;
	ServiceContainer m_container;
};


using RHIImplementations = ::testing::Types<
	VulkanTag
#ifdef OS_WINDOWS
	,DirectX12Tag
#endif
>;
TYPED_TEST_SUITE(RHITest, RHIImplementations);