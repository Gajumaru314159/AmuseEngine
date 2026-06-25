//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/RHI/All.h>
#include <Amuse/RPI/All.h>
#ifdef OS_WINDOWS
#include <Amuse/DirectX12RHI/System.h>
#include <Amuse/DirectX12RHI/DirectX12RHIConfig.h>
#endif
#include <Amuse/VulkanRHI/System.h>
#include <Amuse/VulkanRHI/VulkanRHIConfig.h>
#include <Amuse/Platform/System.h>
#include <magic_enum.hpp>

using namespace Amuse;
using namespace Amuse;
using namespace Amuse;

namespace Amuse {
	class SystemResource;
}

#ifdef OS_WINDOWS
struct DirectX12Tag{};
#endif
struct VulkanTag{};

template<typename T>
class RPITestBase : public ::testing::Test {
protected:
	RPITestBase(bool bindless) {
		m_config.enableBindless = bindless;
    #ifdef OS_WINDOWS
		m_dx12config.enableDebugLayer = true;
		m_dx12config.breakWithWarning = true;
    #endif
		m_vkconfig.enableDebugLayer = true;
	}

	virtual void SetUp() {

		ServiceInjector injector;
		ServiceContainer container;

#ifdef OS_WINDOWS
		if constexpr (std::is_same_v<T, DirectX12Tag>) {
			RegisterDirectX12RHIService(injector);
		}
#endif
		if constexpr (std::is_same_v<T, VulkanTag>) {
			RegisterVulkanRHIService(injector);
		}

		RegisterRHIService(injector);
		RegisterGraphicsService(injector);

		injector.bind(m_config);
#ifdef OS_WINDOWS
		injector.bind(m_dx12config);
#endif
		injector.bind(m_vkconfig);

		injector.createAll<Amuse::SystemResource, Amuse::Graphics>(m_container);

	};
	virtual void TearDown() {};

public:
	RHIConfig m_config;
#ifdef OS_WINDOWS
	DirectX12RHIConfig m_dx12config;
#endif
	VulkanRHIConfig m_vkconfig;
	ServiceContainer m_container;
};

template<typename T>
class BindlessRPITest : public RPITestBase<T> {
public:
	BindlessRPITest() : RPITestBase<T>(true) {
	}
};
template<typename T>
class BindfullRPITest : public RPITestBase<T> {
public:
	BindfullRPITest() : RPITestBase<T>(false) {
	}
};


#ifdef OS_WINDOWS
using RHIImplementations = ::testing::Types<DirectX12Tag, VulkanTag>;
#else
using RHIImplementations = ::testing::Types<VulkanTag>;
#endif
TYPED_TEST_SUITE(BindlessRPITest, RHIImplementations);
TYPED_TEST_SUITE(BindfullRPITest, RHIImplementations);
