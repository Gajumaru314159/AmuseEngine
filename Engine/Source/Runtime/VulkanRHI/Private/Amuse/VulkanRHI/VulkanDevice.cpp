//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/VulkanRHI/VulkanDevice.h>
#include <Amuse/VulkanRHI/Utility/TypeConverter.h>
#include <Amuse/VulkanRHI/Buffer/VulkanBuffer.h>
#include <Amuse/VulkanRHI/Texture/VulkanTexture.h>
#include <Amuse/VulkanRHI/SwapChain/VulkanSwapChain.h>
#include <Amuse/VulkanRHI/Descriptor/VulkanDescriptorLayout.h>
#include <Amuse/VulkanRHI/Descriptor/VulkanDescriptorTable.h>
#include <Amuse/VulkanRHI/Command/VulkanCommandList.h>
#include <Amuse/VulkanRHI/Command/VulkanCommandQueue.h>
#include <Amuse/VulkanRHI/Shader/VulkanShader.h>
#include <Amuse/VulkanRHI/Sampler/VulkanSampler.h>
#include <Amuse/VulkanRHI/RootSignature/VulkanRootSignature.h>
#include <Amuse/VulkanRHI/PipelineState/VulkanPipelineState.h>
#include <Amuse/VulkanRHI/PipelineState/VulkanComputePipelineState.h>
#include <Amuse/VulkanRHI/Query/VulkanTimerQuery.h>
#include <Amuse/VulkanRHI/Fence/VulkanFence.h>
#include <Amuse/VulkanRHI/Buffer/VulkanBufferUploader.h>
#include <Amuse/VulkanRHI/Descriptor/VulkanDescriptorHeap.h>
#include <Amuse/Core/Misc/ErrorCode.h>

#define SAFE_CREATE(type,type_impl,...)			\
		try {\
			return new type_impl(__VA_ARGS__);\
		} catch (const vk::Error& error) {\
			LOG_ERROR("[VulkanDevice] {}の構築に失敗 {}", #type,error.what());\
			return nullptr;\
		} catch (const std::exception& error) {\
			LOG_ERROR("[VulkanDevice] {}の構築に失敗 {}", #type,error.what());\
			return nullptr;\
		}

namespace Amuse {
	//@―---------------------------------------------------------------------------
	//! @brief  デバッグレイヤのコールバック
	//@―---------------------------------------------------------------------------
#if defined(OS_WINDOWS)
	static VkBool32 DebugUtilsMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,vk::DebugUtilsMessageTypeFlagsEXT messageTypes,const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,void* pUserData) {
		if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
			LOG_ERROR("[VulkanDevice] {}", pCallbackData->pMessage);
			CallBreakPoint();
		}
		if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
			LOG_WARNING("[VulkanDevice] {}", pCallbackData->pMessage);
			CallBreakPoint();
		}
		if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
			LOG_INFO("[VulkanDevice] {}", pCallbackData->pMessage);
		}
		if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
			LOG_TRACE("[VulkanDevice] {}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}
#else
	static VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,VkDebugUtilsMessageTypeFlagsEXT messageTypes,const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,void* pUserData) {
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			LOG_ERROR("[VulkanDevice] {}", pCallbackData->pMessage);
			CallBreakPoint();
		}
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			LOG_WARNING("[VulkanDevice] {}", pCallbackData->pMessage);
			CallBreakPoint();
		}
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			LOG_INFO("[VulkanDevice] {}", pCallbackData->pMessage);
		}
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			LOG_TRACE("[VulkanDevice] {}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}
#endif

}

namespace Amuse {
	//@―---------------------------------------------------------------------------
	//! @brief  コンストラクタ
	//@―---------------------------------------------------------------------------
	VulkanDevice::VulkanDevice(WindowManager&, GraphicObjectManager& objectManager, const Amuse::RHIConfig* config, const VulkanRHIConfig* vconfig)
		: Device(objectManager, config)
		, m_config(config ? *config : Amuse::RHIConfig{})
		, m_vconfig(vconfig ? *vconfig : Amuse::VulkanRHIConfig{})
	{
		createInstance();
		createPhysicalDevice();
		createDevice();
		createQueue();
		createUploaders();
		createShaderCompiler();
		initializeBindless();
	}

	//@―---------------------------------------------------------------------------
	//! @brief  デストラクタ
	//@―---------------------------------------------------------------------------
	VulkanDevice::~VulkanDevice() {

		clearCommands();

		m_copyCommandList = {};
		m_textureUploader = {};
		m_bufferUploader = {};

		finalize();

		m_descriptorHeap = {};
	}


	//@―---------------------------------------------------------------------------
	//! @brief  妥当な状態か
	//@―---------------------------------------------------------------------------
	bool VulkanDevice::isValid()const {
		return
			*m_instance &&
			*m_physicalDevice &&
			*m_device;
	}


	//@―---------------------------------------------------------------------------
	//! @brief  VkInstance生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createInstance() {

		// レイヤー / 拡張機能名
		Vector<const char*> layerNames;
		Vector<const char*> extensionNames;
		if (m_vconfig.enableDebugLayer) {
			AMUSE_DEBUG_CONTEXT(layerNames.push_back("VK_LAYER_KHRONOS_validation"));
			AMUSE_DEBUG_CONTEXT(extensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME));
			AMUSE_DEBUG_CONTEXT(extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
		}
		
		extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		OS_WINDOWS_CONTEXT(extensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME));
		OS_LINUX_CONTEXT(extensionNames.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME));



		// 利用可能なレイヤーでフィルタ
		Vector<const char*> validLayerNames;
		Vector<const char*> invalidLayerNames;
		const auto existLayerNames = vk::enumerateInstanceLayerProperties();
		for (const auto& name : layerNames)
		{
			bool contains = false;
			for (auto prop : existLayerNames) {
				if (strcmp(name, prop.layerName) == 0) {
					contains = true;
					break;
				}
			}
			if (contains) {
				validLayerNames.push_back(name);
			} else {
				invalidLayerNames.push_back(name);
			}
		}

		// 利用可能な拡張機能でフィルタ
		Vector<const char*> validExtensionNames;
		Vector<const char*> invalidExtensionNames;
		const auto existExtensionNames = vk::enumerateInstanceExtensionProperties();
		for (const auto& name : extensionNames)
		{
			bool contains = false;
			for (auto prop : existExtensionNames) {
				if (strcmp(name, prop.extensionName) == 0) {
					contains = true;
					break;
				}
			}
			if (contains) {
				validExtensionNames.push_back(name);
			} else {
				invalidExtensionNames.push_back(name);
			}
		}

		// アプリ情報
		vk::ApplicationInfo appInfo;
		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.pApplicationName = "OctbitEngine";
		appInfo.pEngineName = "OctbitEngine";

		// インスタンス情報
		vk::InstanceCreateInfo instanceInfo;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(validLayerNames.size());
		instanceInfo.ppEnabledLayerNames = validLayerNames.data();
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(validExtensionNames.size());
		instanceInfo.ppEnabledExtensionNames = validExtensionNames.data();

#if AMUSE_DEBUG
		{
			// 初期化情報を出力
			String message;
			message += Format("\n[ Vulkan ver.{} ]\n", m_context.enumerateInstanceVersion());
			message += Format("Validation Layers\n");
			for (auto& name : validLayerNames) {
				message += Format("+ {}\n", name);
			}
			for (auto& name : invalidLayerNames) {
				message += Format("- {}\n", name);
			}
			message += Format("Extensions\n");
			for (auto& name : validExtensionNames) {
				message += Format("+ {}\n", name);
			}
			for (auto& name : invalidExtensionNames) {
				message += Format("- {}\n", name);
			}
			message.pop_back();
			LOG_INFO("{}", message);
		}
#endif

		for (auto name : extensionNames) {
			if (strcmp(name, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
				m_featuresEx.debugMarkerEnabled = true;
			}
		}

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo;
		if (m_vconfig.enableDebugLayer) {
			if (m_vconfig.logLevel >= LogLevel::Error)		debugUtilsCreateInfo.messageSeverity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
			if (m_vconfig.logLevel >= LogLevel::Warning)	debugUtilsCreateInfo.messageSeverity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
			if (m_vconfig.logLevel >= LogLevel::Info)		debugUtilsCreateInfo.messageSeverity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
			if (m_vconfig.logLevel >= LogLevel::Trace)		debugUtilsCreateInfo.messageSeverity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

			debugUtilsCreateInfo.messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;
			debugUtilsCreateInfo.messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
			debugUtilsCreateInfo.messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
			// debugUtilsCreateInfo.messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding; // VK_EXT_device_address_binding_reportが必要

			debugUtilsCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;
			debugUtilsCreateInfo.pUserData = nullptr;
			instanceInfo.pNext = &debugUtilsCreateInfo;
		}

		m_instance = m_context.createInstance(instanceInfo, m_allocationCallbacks);

	}


	//@―---------------------------------------------------------------------------
	//! @brief  VkPhysicalDevice生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createPhysicalDevice() {

		auto devices = m_instance.enumeratePhysicalDevices();

		if (devices.empty()) {
			throw Exception("GPUが接続されていません。");
		}

		// キューファミリーチェック
		for (auto& device : devices) {

			// 拡張機能チェック
			// auto featuresProperties = device.getFeatures();
			auto familyPropertyList = device.getQueueFamilyProperties();

			u32 index = 0;
			for (auto& familyProperty : familyPropertyList) {

				bool ok =
					(familyProperty.queueFlags & vk::QueueFlagBits::eGraphics) &&
					(familyProperty.queueFlags & vk::QueueFlagBits::eCompute) &&
					(familyProperty.queueFlags & vk::QueueFlagBits::eTransfer) &&
					(familyProperty.queueFlags & vk::QueueFlagBits::eSparseBinding);

				if (ok == false)
					continue;

				m_queueFamilyIndex = index;
				m_queueCount = familyProperty.queueCount;
				index++;
			}

			// 選択
			m_physicalDevice = device;

			break;
		}

		if (!*m_physicalDevice) {
			LOG_ERROR("GPUが見つかりません。");
			return;
		}

		m_features = m_physicalDevice.getFeatures();
		m_limits = m_physicalDevice.getProperties().limits;

		m_memoryProperties = m_physicalDevice.getMemoryProperties();
	}

	//@―---------------------------------------------------------------------------
	//! @brief  VkDevice生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createDevice() {

		if (!*m_physicalDevice)
			return;

		Vector<const char*> layerNames;
		Vector<const char*> extensionNames;


		AMUSE_DEBUG_CONTEXT(layerNames.push_back("VK_LAYER_KHRONOS_validation"));

		extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensionNames.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
		extensionNames.push_back(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
		extensionNames.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		if (m_config.enableBindless) {
			extensionNames.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
			extensionNames.push_back(VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME);
		}

		// 利用可能なレイヤーでフィルタ
		Vector<const char*> validLayerNames;
		Vector<const char*> invalidLayerNames;
		const auto existLayerNames = m_physicalDevice.enumerateDeviceLayerProperties();
		for (const auto& name : layerNames)
		{
			bool contains = false;
			for (auto prop : existLayerNames) {
				if (strcmp(name, prop.layerName) == 0) {
					contains = true;
					break;
				}
			}

			if (contains) {
				validLayerNames.push_back(name);
			}
			else {
				invalidLayerNames.push_back(name);
			}
		}

		// 利用可能な拡張機能でフィルタ
		Vector<const char*> validExtensionNames;
		Vector<const char*> invalidExtensionNames;
		const auto existExtensionNames = m_physicalDevice.enumerateDeviceExtensionProperties();
		for (const auto& name : extensionNames)
		{
			bool contains = false;
			for (auto prop : existExtensionNames) {
				if (strcmp(name, prop.extensionName) == 0) {
					contains = true;
					break;
				}
			}

			if (contains) {
				validExtensionNames.push_back(name);
			}
			else {
				invalidExtensionNames.push_back(name);
			}
		}

#if AMUSE_DEBUG
		{
			// 初期化情報を出力
			String message;
			message += Format("Device Validation Layers\n");
			for (auto& name : validLayerNames) {
				message += Format("+ {}\n", name);
			}
			for (auto& name : invalidLayerNames) {
				message += Format("- {}\n", name);
			}
			message += Format("Device Extensions\n");
			for (auto& name : validExtensionNames) {
				message += Format("+ {}\n", name);
			}
			for (auto& name : invalidExtensionNames) {
				message += Format("- {}\n", name);
			}
			message.pop_back();
			LOG_INFO("{}", message);
		}
#endif

		// デバイスキューのパラメータ
		Vector<float> queuePriorities(m_queueCount, 0.0f);
		vk::DeviceQueueCreateInfo queueInfo;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = queuePriorities.data();
		queueInfo.queueFamilyIndex = m_queueFamilyIndex;
		queueInfo.queueCount = static_cast<uint32_t>(queuePriorities.size());

		// 必要なデバイス機能
		vk::PhysicalDeviceFeatures deviceFeatures{};
		if (m_features.imageCubeArray) {
			deviceFeatures.imageCubeArray = VK_TRUE;
		} else {
			LOG_WARNING("このデバイスはimageCubeArrayをサポートしていません。キューブマップ配列は使用できません。");
		}
		if (m_features.tessellationShader) {
			deviceFeatures.tessellationShader = VK_TRUE;
		}
		if (m_features.geometryShader) {
			deviceFeatures.geometryShader = VK_TRUE;
		}

		// 生成情報
		vk::DeviceCreateInfo info;
		info.queueCreateInfoCount = 1;
		info.pQueueCreateInfos = &queueInfo;
		info.enabledExtensionCount = static_cast<uint32_t>(validExtensionNames.size());
		info.ppEnabledExtensionNames = validExtensionNames.data();
		info.enabledLayerCount = static_cast<uint32_t>(validLayerNames.size());
		info.ppEnabledLayerNames = validLayerNames.data();
		info.pEnabledFeatures = &deviceFeatures;

		// Dynamic Rendering機能を有効にするための構造体
		vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
		dynamicRenderingFeatures.dynamicRendering = true;
		info.pNext = &dynamicRenderingFeatures;

		// Mutable Descriptor Type
		vk::PhysicalDeviceMutableDescriptorTypeFeaturesEXT mutableDescriptorTypeFeature{};
		mutableDescriptorTypeFeature.mutableDescriptorType = m_config.enableBindless;
		dynamicRenderingFeatures.pNext = &mutableDescriptorTypeFeature;


		vk::PhysicalDeviceVulkan12Features vulkan12Features{};
		vulkan12Features.timelineSemaphore = true;
		vulkan12Features.runtimeDescriptorArray = true;
		vulkan12Features.descriptorIndexing = true;
		vulkan12Features.descriptorBindingPartiallyBound = true;
		mutableDescriptorTypeFeature.pNext = &vulkan12Features;

		m_device = m_physicalDevice.createDevice(info, m_allocationCallbacks);

	}


	//@―---------------------------------------------------------------------------
	//! @brief  VkQueue生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createQueue() {

		if (!*m_device)
			return;

		m_commandQueue = std::make_unique<VulkanCommandQueue>(*this);
	}


	//@―---------------------------------------------------------------------------
	//! @brief  VkQueue生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createUploaders() {
		m_bufferUploader = std::make_unique<VulkanBufferUploader>(*this,4*1024);
		m_textureUploader = std::make_unique<VulkanTextureUploader>(*this);

		m_copyCommandList = createCommandList(CommandListDesc{ "CopyBuffer",CommandListType::Graphic});
	}


	//@―---------------------------------------------------------------------------
	//! @brief  ShaderCompiler生成
	//@―---------------------------------------------------------------------------
	void VulkanDevice::createShaderCompiler() {

		HRESULT result;

		result = ::DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_shaderCompiler));
		if (FAILED(result)) {
			LOG_ERROR("DxcCreateInstance() {}",ErrorCode(result));
			return;
		}

		result = ::DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_shaderUtils));
		if (FAILED(result)) {
			LOG_ERROR("DxcCreateInstance() {}", ErrorCode(result));
			return;
		}

		// NOTE FileIOをフックする場合は、IDxcIncludeHandlerを継承したカスタムハンドラーを生成する
		result = m_shaderUtils->CreateDefaultIncludeHandler(&m_shaderIncludeHandler);
		if (FAILED(result)) {
			LOG_ERROR("CreateDefaultIncludeHandler() {}", ErrorCode(result));
			return;
		}
	}

	//@―---------------------------------------------------------------------------
	//! @brief  バインドレス初期化
	//@―---------------------------------------------------------------------------
	void VulkanDevice::initializeBindless() {

		if (m_config.enableBindless) {
			m_descriptorHeap = std::make_unique<VulkanDescriptorHeap>(
				*this, 
				std::min<s32>(m_limits.maxDescriptorSetSampledImages,10000), 
				std::min<s32>(m_limits.maxDescriptorSetSamplers,100000)
			);
		}
	}


	//@―---------------------------------------------------------------------------
	//! @brief  コマンドをシステムキューに追加
	//@―---------------------------------------------------------------------------
	void VulkanDevice::entryCommandList(const Ref<CommandList>& commandList) {
		m_commandQueue->entryCommandList(commandList);
	}

	//! @brief  フェンスを生成
	Ref<Fence> VulkanDevice::createFence(const FenceDesc& desc) {
		SAFE_CREATE(Fence, VulkanFence, *this, desc);
	}

	//! @brief  コマンドリストを実行
	u64 VulkanDevice::executeCommandLists(Span<Ref<CommandList>> commandLists, CommandQueueType) {
		for (const auto& commandList : commandLists) {
			if (commandList) {
				m_commandQueue->entryCommandList(commandList);
			}
		}
		const auto fenceValue = m_commandQueue->execute();
		for (const auto& commandList : commandLists) {
			if (auto native = commandList.cast<VulkanCommandList>()) {
				for (const auto& query : native->getEventQueries()) {
					if (auto event = query.cast<VulkanEventQuery>()) {
						event->signal(fenceValue);
					}
				}
				native->clearEventQueries();
			}
		}
		return fenceValue;
	}

	//! @brief  キューがアイドルになるまで待機
	void VulkanDevice::waitForIdle(CommandQueueType) {
		m_commandQueue->wait();
	}

	//! @brief  キュー間同期を設定
	void VulkanDevice::queueWait(CommandQueueType, CommandQueueType, u64 fenceValue) {
		m_commandQueue->wait(fenceValue);
	}

	//! @brief ビデオカード情報を取得  
	Vector<VideoCard> VulkanDevice::getVideoCards() const {

		Vector<VideoCard> videoCards;
		auto devices = m_instance.enumeratePhysicalDevices();
		for (auto& device : devices) {
			auto properties = device.getProperties(); 
			auto memoryProperties = device.getMemoryProperties();
			VideoCard videoCard;
			videoCard.name = properties.deviceName.data();
			videoCard.deviceId = properties.deviceID;
			videoCard.memory = 0;
			for (auto& heap : memoryProperties.memoryHeaps) {
				if (heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
					videoCard.memory.size += heap.size;
				}
			}

			LOG_INFO("Vulkanはモニター情報の取得をまだ実装していません。");

			videoCards.push_back(videoCard);
		}
		return videoCards;

	}


	//@―---------------------------------------------------------------------------
	//! @brief  更新
	//@―---------------------------------------------------------------------------
	void VulkanDevice::update() {

		{

			m_copyCommandList->begin();
			m_bufferUploader->update(m_copyCommandList);
			m_textureUploader->update(m_copyCommandList);
			m_copyCommandList->end();
		
			m_commandQueue->entryCommandListTop(m_copyCommandList);
			// m_copyCommandList->wait();
		}

		if (m_descriptorHeap) m_descriptorHeap->update();
		m_commandQueue->execute();
		m_commandQueue->wait();

		Device::update();
	}



	//! @brief  スワップ・チェーンを生成
	Ref<SwapChain> VulkanDevice::createSwapChain(const SwapChainDesc& desc) {
		SAFE_CREATE(SwapChain, VulkanSwapChain,*this, desc);
	}


	//! @brief  コマンドリスト生成
	Ref<CommandList> VulkanDevice::createCommandList(const CommandListDesc& desc) {
		SAFE_CREATE(CommandList, VulkanCommandList, *this, desc);
	}



	//! @brief  ルートシグネチャを生成
	Ref<RootSignature> VulkanDevice::createRootSignature(const RootSignatureDesc& desc) {
		SAFE_CREATE(RootSignature, VulkanRootSignature, *this,desc);
	}


	//! @brief  パイプラインステートを生成
	Ref<PipelineState> VulkanDevice::createPipelineState(const PipelineStateDesc& desc) {
		SAFE_CREATE(PipelineState, VulkanPipelineState,*this, desc);
	}


	//! @brief  コンピュートパイプラインステートを生成
	Ref<ComputePipelineState> VulkanDevice::createComputePipelineState(const ComputePipelineStateDesc& desc) {
		SAFE_CREATE(ComputePipelineState, VulkanComputePipelineState, *this, desc);
	}


	//! @brief  テクスチャを生成
	Ref<Texture> VulkanDevice::createTexture(const TextureDesc& desc) {
		SAFE_CREATE(Texture, VulkanTexture, *this, desc);
	}


	Ref<Texture> VulkanDevice::createTexture(StringView name, TextureType type, Size size, Span<const IntColor> colors) { 
		try {
			Ref<Texture> texture = new VulkanTexture(*this, name, type, size, colors);
			update();
			return texture;
		} catch (const vk::Error& error) {
			LOG_ERROR("[VulkanDevice] Textureの構築に失敗 {}", error.what());
			return nullptr;
		} catch (const std::exception& error) {
			LOG_ERROR("[VulkanDevice] Textureの構築に失敗 {}", error.what());
			return nullptr;
		}
	}


	//! @brief  テクスチャを生成
	Ref<Texture> VulkanDevice::createTexture(StringView name, BlobView blob) {
		try {
			Ref<Texture> texture = new VulkanTexture(*this, name, blob);
			update();
			return texture;
		} catch (const vk::Error& error) {
			LOG_ERROR("[VulkanDevice] Textureの構築に失敗 {}", error.what());
			return nullptr;
		} catch (const std::exception& error) {
			LOG_ERROR("[VulkanDevice] Textureの構築に失敗 {}", error.what());
			return nullptr;
		}
	}

	//! @brief  テクスチャビューを生成
	Ref<Texture> VulkanDevice::createTexture(const TextureViewDesc& desc) {
		SAFE_CREATE(Texture, VulkanTexture, *this, desc);
	}


	//! @brief  レンダーテクスチャを生成
	Ref<RenderTexture> VulkanDevice::createRenderTexture(const RenderTextureDesc& desc) {
		SAFE_CREATE(Texture, VulkanTexture, *this, desc);
	}


	//! @brief  サンプラーを生成
	Ref<Sampler> VulkanDevice::createSampler(const SamplerDesc& desc) { 
		SAFE_CREATE(Sampler, VulkanSampler, *this, desc);
	}


	//! @brief  バッファーを生成
	Ref<Buffer> VulkanDevice::createBuffer(const BufferDesc& desc) { 
		SAFE_CREATE(Buffer, VulkanBuffer, *this, desc);
	}
	Ref<Buffer> VulkanDevice::createBuffer(const BufferViewDesc& desc) { 
		SAFE_CREATE(Buffer, VulkanBuffer, *this, desc);
	}


	//! @brief  シェーダをコンパイル
	Ref<Shader> VulkanDevice::compileShader(const ShaderCompileDesc& desc) { 
		if (!supports(desc.stage)) {
			LOG_ERROR("非対応のShaderStageです。Shader::Supports()でサポート状況を確認してください。");
			return nullptr;
		}
		SAFE_CREATE(Shader, VulkanShader, *this, desc);
	}


	//! @brief  シェーダをロード
	Ref<Shader> VulkanDevice::loadShader(BlobView, ShaderStage) { return {}; }


	//! @brief  デスクリプタ・テーブルを生成
	Ref<DescriptorLayout> VulkanDevice::createDescriptorLayout(const DescriptorLayoutDesc& desc) { 
		SAFE_CREATE(DescriptorLayout, VulkanDescriptorLayout, *this, desc);
	}
	Ref<DescriptorTable> VulkanDevice::createDescriptorTable(const DescriptorTableDesc& desc) {
		SAFE_CREATE(DescriptorTable, VulkanDescriptorTable, *this, desc);
	}

	//! @brief GPUタイマークエリを生成
	Ref<TimerQuery> VulkanDevice::createTimerQuery() {
		SAFE_CREATE(TimerQuery, VulkanTimerQuery, *this);
	}

	//! @brief GPUイベントクエリを生成
	Ref<EventQuery> VulkanDevice::createEventQuery() {
		SAFE_CREATE(EventQuery, VulkanEventQuery, *this);
	}

	//! @brief GPUイベントクエリが完了済みか
	bool VulkanDevice::pollEventQuery(const Ref<EventQuery>& query) {
		if (auto event = query.cast<VulkanEventQuery>()) {
			return m_commandQueue->isCompleted(event->getFenceValue());
		}
		return false;
	}

	//! @brief GPUイベントクエリを待機
	void VulkanDevice::waitEventQuery(const Ref<EventQuery>& query) {
		if (auto event = query.cast<VulkanEventQuery>()) {
			const auto fenceValue = event->getFenceValue();
			if (fenceValue != 0) {
				m_commandQueue->wait(fenceValue);
			}
		}
	}

	//! @brief GPUタイマークエリが取得可能か
	bool VulkanDevice::pollTimerQuery(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<VulkanTimerQuery>()) {
			return timer->isResultAvailable();
		}
		return false;
	}

	//! @brief GPUタイマークエリの計測時間を取得
	f32 VulkanDevice::getTimerQueryTime(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<VulkanTimerQuery>()) {
			return timer->getTime();
		}
		return 0.0f;
	}


	//! @brief サポートしているテクスチャフォーマットか 
	bool VulkanDevice::supports(TextureFormat format, TextureType type)const {

		try {
			vk::ImageFormatProperties properties = m_physicalDevice.getImageFormatProperties(
				TypeConverter::Convert(format),
				TypeConverter::Convert(type),
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eSampled,
				vk::ImageCreateFlagBits{}
			);
			return true;
		}
		catch (const std::exception& e) {
			return false;
		}

	}
	//! @brief サポートしているレンダーテクスチャフォーマットか 
	bool VulkanDevice::supportsForRenderTexture(TextureFormat format)const {

		// そもそも2Dテクスチャに対応していない場合は不可
		if (!supports(format,TextureType::Texture2D)) return false;

		try {
			vk::ImageUsageFlags flags{};
			if (TextureFormatUtility::HasColor(format)) {
				flags |= vk::ImageUsageFlagBits::eColorAttachment;
			}
			if (TextureFormatUtility::HasDepth(format)) {
				flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
			}

			vk::ImageFormatProperties properties = m_physicalDevice.getImageFormatProperties(
				TypeConverter::Convert(format),
				vk::ImageType::e2D,
				vk::ImageTiling::eOptimal,
				flags,
				vk::ImageCreateFlagBits{}
			);
			return true;
		}
		catch (const std::exception& e) {
			return false;
		}

	}
	//! @brief サポートしているシェーダーステージか
	bool VulkanDevice::supports(ShaderStage stage)const {
		if (stage == ShaderStage::Hull && !m_features.tessellationShader) {
			return false;
		}
		if (stage == ShaderStage::Domain && !m_features.tessellationShader) {
			return false;
		}
		if (stage == ShaderStage::Geometry && !m_features.geometryShader) {
			return false;
		}

		return true;
	}

	//! @brief サポートしているRHI機能を取得
	RHIFeatures VulkanDevice::getFeatures() const {
		RHIFeatures features;
		features.computeQueue = true;
		features.copyQueue = true;
		features.indirectDraw = true;
		features.indirectDispatch = true;
		features.bindless = m_config.enableBindless && m_descriptorHeap != nullptr;
		return features;
	}

	//! @brief 指定用途でサポートしているテクスチャフォーマットか
	bool VulkanDevice::supports(TextureFormat format, FormatSupport usage) const {
		if (format == TextureFormat::Unknown) return false;

		vk::FormatFeatureFlags required;
		switch (usage) {
		case FormatSupport::Texture:
		case FormatSupport::ShaderResource:
			required = vk::FormatFeatureFlagBits::eSampledImage;
			break;
		case FormatSupport::RenderTarget:
			required = vk::FormatFeatureFlagBits::eColorAttachment;
			break;
		case FormatSupport::DepthStencil:
			required = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
			break;
		case FormatSupport::UnorderedAccess:
			required = vk::FormatFeatureFlagBits::eStorageImage;
			break;
		case FormatSupport::Copy:
			required = vk::FormatFeatureFlags(VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
			break;
		case FormatSupport::Resolve:
			required = vk::FormatFeatureFlagBits::eColorAttachment;
			break;
		default:
			return false;
		}

		auto properties = m_physicalDevice.getFormatProperties(TypeConverter::Convert(format));
		return (properties.optimalTilingFeatures & required) == required;
	}

	void VulkanDevice::allocateHandle(VulkanDescriptorHandle& handle, vk::WriteDescriptorSet& desc) {
		if (m_descriptorHeap) {
			m_descriptorHeap->allocateHandle(handle, desc);
		}
	}


	void VulkanDevice::setDescriptorHeaps(const vk::raii::CommandBuffer& commandBuffer, const vk::PipelineLayout& layout, s32 slot) {
		if (m_descriptorHeap) {
			m_descriptorHeap->recordDescriptorHeap(commandBuffer, layout, slot);
		}
	}

	vk::DescriptorSetLayout VulkanDevice::getBindlessDescriptorSetLayout() const {
		return m_descriptorHeap->getLayout();
	}


	void VulkanDevice::clearCommands() const {
		m_commandQueue->execute();
		m_commandQueue->wait();
	}

}
