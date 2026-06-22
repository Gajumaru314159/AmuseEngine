//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

//============================================
// System Base
//============================================
#include <Amuse/Core/Core.h>


//============================================
// Vulkan
//============================================
#ifdef OS_WINDOWS
#	define VK_USE_PLATFORM_WIN32_KHR
#elif defined(OS_LINUX)
#	define VK_USE_PLATFORM_XLIB_KHR
#endif

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <vulkan/vk_layer.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_raii.hpp>

#include <Amuse/VulkanRHI/Utility/Utility.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;
	class VulkanDevice;
}

//============================================
// DirectX Compiler
//============================================
#ifdef OS_WINDOWS
#pragma warning(disable:4100)
#include <d3d12shader.h>
#include <dxcapi.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif
#ifdef OS_LINUX
#include <dxcapi.h>
template<class T>
using ComPtr = CComPtr<T>;
#undef None
#undef Always
#undef Success
#endif