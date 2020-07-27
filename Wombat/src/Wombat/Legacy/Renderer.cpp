#include "Renderer.h"

namespace Wombat
{
	Renderer::Renderer(const char* appName, bool isDebugEnabled = true)
	{
		_isDebugEnabled = isDebugEnabled;
		_requiredVkValidationLayers =
		{
			"VK_LAYER_KHRONOS_validation" };

		std::cout << "Renderer ctor" << std::endl;

		//guard for GLFW
		//TODO figure out what to do if GLFW not used
		if (!glfwInit())
			throw std::runtime_error("GLFW failed to init");

		//TODO figure out what to do if GLFW is not used
		uint32_t extensionCount = 0;
		// std::vector<const char*> extensionNames = {"VK_KHR_surface"};
		const char** extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);

		_requiredVkInstanceExtProps = std::vector<const char*>{ *extensionNames };

		if (isDebugEnabled)
		{
			_requiredVkInstanceExtProps.push_back("VK_EXT_debug_utils");
		}

#ifdef WOMBAT_WINDOWS
		_requiredVkInstanceExtProps.push_back("VK_KHR_win32_surface");
#endif // WB_PLATFORM_WINDOWS

		for (size_t i = 0; i < _requiredVkInstanceExtProps.size(); i++)
		{
			std::cout << _requiredVkInstanceExtProps[i] << std::endl;
		}

		_requiredVkDeviceExtProps =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		Renderer::CreateInstance(appName, isDebugEnabled);

		if (isDebugEnabled)
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
			createInfo.pUserData = nullptr; // Optional

			if (CreateDebugUtilsMessengerEXT(_vkInstanceHandle, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to set up debug messenger!");
			}
		}
	}

	Renderer::~Renderer()
	{
		std::cout << "Renderer destruction" << std::endl;

		for (auto image : _images)
		{
			vkDestroyImageView(_logicalDevice, image.imageView, nullptr);
		}


		vkFreeCommandBuffers(_logicalDevice, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

		size_t size = _commandBufferFences.size();
		for (size_t i = 0; i < size; i++)
		{
			vkDestroyFence(_logicalDevice, _commandBufferFences[i], nullptr);
		}
		size = _acquireSemaphores.size();
		printf("_logicalDevice: %p\n", (void*)_logicalDevice);

		printf("_acquireSemaphores: %p\n", (void*)&_acquireSemaphores);
		for (size_t i = 0; i < size; i++)
		{
			printf("_acquireSemaphores[%zd]: %p\n", i, (void*)_acquireSemaphores[i]);

			vkDestroySemaphore(_logicalDevice, _acquireSemaphores[i], nullptr);
		}
		size = _renderCompleteSemaphores.size();

		printf("_renderCompleteSemaphores: %p\n", (void*)&_renderCompleteSemaphores);
		for (size_t i = 0; i < size; i++)
		{
			printf("_renderCompleteSemaphores[%zd]: %p\n", i, (void*)_renderCompleteSemaphores[i]);

			vkDestroySemaphore(_logicalDevice, _renderCompleteSemaphores[i], nullptr);
		}

		vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);
		vkDestroySwapchainKHR(_logicalDevice, _vkSwapchain, nullptr);

		vkDestroyDevice(_logicalDevice, nullptr);
		vkDestroySurfaceKHR(_vkInstanceHandle, _vkSurface, nullptr);
		DestroyDebugUtilsMessengerEXT(_vkInstanceHandle, _debugMessenger, nullptr);
		vkDestroyInstance(_vkInstanceHandle, nullptr);
		glfwTerminate();
	}

	std::vector<VkLayerProperties> Renderer::GetValidationLayerProperties()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		return availableLayers;
	}

	bool Renderer::CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
		std::vector<const char*> requestedLayers)
	{
		//for each requested layer
		for (const char* requestedLayer : requestedLayers)
		{
			bool hasRequested = false;

			//for each available layer
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(requestedLayer, layerProperties.layerName) == 0)
				{
					hasRequested = true;
					break;
				}
			}

			//guard
			if (!hasRequested)
				return false;
		}

		//if we're not returned early, all requested layers exist
		return true;
	}

	bool Renderer::HasPhysicalDeviceExtensionSupport(PhysicalGpu* gpu,
		std::vector<const char*>* requiredDeviceExtProps)
	{
		std::vector<const char*> requiredArray = *requiredDeviceExtProps;
		std::vector<const char*> supportedArray;

		for (size_t i = 0; i < gpu->extProps.size(); i++)
		{
			supportedArray.push_back(gpu->extProps[i].extensionName);
		}

		//TODO get smarter and use transform
		//supportedArray.resize(gpu->extProps.size());
		////transform extProps into extensionNames
		//std::transform(gpu->extProps.begin(), gpu->extProps.end(), supportedArray.begin(),
		//	[](VkExtensionProperties ext) -> const char* {return ext.extensionName; });

		std::vector<const char*> diff;

		//for each required ext
		for (size_t i = 0; i < requiredArray.size(); i++)
		{
			bool hasExt = false;
			//check if we have it
			for (size_t j = 0; j < supportedArray.size(); j++)
			{
				if (strcmp(requiredArray[i], supportedArray[j]) == 0)
					hasExt = true;
			}

			if (!hasExt)
			{
				diff.push_back(requiredArray[i]);
			}
		}

		//TODO get smarter and use set_difference
		/*std::set_difference(requiredArray.begin(), requiredArray.end(),
			supportedArray.begin(), supportedArray.end(),
			std::inserter(diff, diff.begin()));*/

			/*std::set_difference(supportedArray.begin(), supportedArray.end(),
				requiredArray.begin(), requiredArray.end(),
				std::inserter(diff, diff.begin()));*/

		if (diff.size() > 0)
		{
			return false;
		}
		else
		{
			return true;
		}

	}

	void Renderer::CreateInstance(const char* appName, bool isDebugEnabled)
	{
		//TODO move into config module
		VkApplicationInfo vkAppInfo{};
		vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vkAppInfo.pEngineName = "WombatEngine";
		vkAppInfo.apiVersion = VK_API_VERSION_1_2;
		vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
		vkAppInfo.pApplicationName = appName;

		//TODO move into config module
		VkInstanceCreateInfo vkInstanceInfo{};
		vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkInstanceInfo.pApplicationInfo = &vkAppInfo;
		//validation layers
		if (!isDebugEnabled)
		{
			vkInstanceInfo.enabledLayerCount = 0;
		}
		else
		{
			//TODO move this into config
			bool allRequestedLayersAvailable = CheckAllLayersSupported(GetValidationLayerProperties(), _requiredVkValidationLayers);
			if (!allRequestedLayersAvailable)
			{
				throw std::runtime_error("One or more requested validation layers not available.");
			}

			vkInstanceInfo.enabledLayerCount = static_cast<uint32_t>(_requiredVkValidationLayers.size());
			vkInstanceInfo.ppEnabledLayerNames = _requiredVkValidationLayers.data();
		}
		//extensions

		vkInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(_requiredVkInstanceExtProps.size());
		vkInstanceInfo.ppEnabledExtensionNames = _requiredVkInstanceExtProps.data();

		if (vkCreateInstance(&vkInstanceInfo, nullptr, &_vkInstanceHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create instance");
	}

	void Renderer::CreateSemaphores(int numFrameData)
	{
		VkSemaphoreCreateInfo semaInfo = {};
		semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		_acquireSemaphores.resize(numFrameData);
		_renderCompleteSemaphores.resize(numFrameData);

		for (size_t i = 0; i < numFrameData; i++)
		{
			vkCreateSemaphore(_logicalDevice, &semaInfo, nullptr, &_acquireSemaphores[i]);
			vkCreateSemaphore(_logicalDevice, &semaInfo, nullptr, &_renderCompleteSemaphores[i]);
		}
	}

	void Renderer::CreateCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		commandPoolCreateInfo.queueFamilyIndex = _graphicsFamilyId;

		vkCreateCommandPool(_logicalDevice, &commandPoolCreateInfo, nullptr, &_commandPool);
	}

	void Renderer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandPool = _commandPool;
		commandBufferAllocateInfo.commandBufferCount = _NUM_FRAME_DATA;

		_commandBuffers.resize(_NUM_FRAME_DATA);

		vkAllocateCommandBuffers(_logicalDevice, &commandBufferAllocateInfo, _commandBuffers.data());

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		_commandBufferFences.resize(_NUM_FRAME_DATA);

		for (size_t i = 0; i < _NUM_FRAME_DATA; i++)
		{
			vkCreateFence(_logicalDevice, &fenceCreateInfo, nullptr, &_commandBufferFences[i]);
		}
	}

	void Renderer::AutoPickSurfaceFormat(std::vector<VkSurfaceFormatKHR>* formats)
	{
		//guard
		if (!(formats->size() > 0)) throw std::runtime_error("No formats");

		for (size_t i = 0; i < formats->size(); i++)
		{
			if (formats[i].data()->format == VK_FORMAT_UNDEFINED)
				throw std::runtime_error("Unknown format");

			if (formats[i].data()->format == VK_FORMAT_B8G8R8A8_UNORM &&
				formats[i].data()->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				_vkSurfaceFormat = *formats[i].data();
				return;
			}
		}

		throw std::runtime_error("No acceptable format");
	}

	void Renderer::OpenDebugWindow()
	{
		try
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			_glfwDebugWindow = glfwCreateWindow(640, 480, "Wombat Debug", NULL, NULL);
			auto res = glfwCreateWindowSurface(_vkInstanceHandle, _glfwDebugWindow, NULL, &_vkSurface);
			// if (glfwCreateWindowSurface(_vkInstanceHandle, _glfwDebugWindow, NULL, &_vkSurface) != VK_SUCCESS)
			// {
			// 	throw std::runtime_error("GLFW failed to create surface");
			// }

			try
			{
				//TODO better architecture for this stuff so individual windows can be created
				UpdatePhysicalDeviceList();
				AutoSelectBestGpu();
				CreateLogicalDeviceAndQueue();
				CreateSemaphores(_NUM_FRAME_DATA);
				CreateCommandPool();
				CreateCommandBuffer();
				CreateSwapchain();


				VkPhysicalDeviceMemoryProperties2 memoryProps2{};
				memoryProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
				
				vkGetPhysicalDeviceMemoryProperties2(_selectedGpu->physicalDevice, &memoryProps2);

					for (size_t i = 0; i < memoryProps2.memoryProperties.memoryHeapCount; i++)
					{
						printf("Heap index: %i -- Heap size: %zd\n\n", i, memoryProps2.memoryProperties.memoryHeaps[i].size);
					}
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

		try
		{
			while (!glfwWindowShouldClose(_glfwDebugWindow))
			{
				try
				{
					glfwPollEvents();
				}
				catch (const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	void Renderer::AutoPickPresentMode(std::vector<VkPresentModeKHR>* presentModes)
	{
		const VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;

		for (auto presentMode : *presentModes)
		{
			if (presentMode == desiredMode)
			{
				_vkPresentMode = presentMode;
			}
		}

		_vkPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	void Renderer::UpdateSurfaceExtent()
	{
		VkSurfaceCapabilitiesKHR surfaceCapa = _selectedGpu->surfaceCapa;

		if (surfaceCapa.currentExtent.width == -1)
		{
			//TODO remove magic numbers
			_extent.width = 600;
			_extent.height = 800;
		}
		else
		{
			_extent = surfaceCapa.currentExtent;
		}
	}

	void Renderer::UpdatePhysicalDeviceList()
	{
		uint32_t numDevices = 0;
		vkEnumeratePhysicalDevices(_vkInstanceHandle, &numDevices, NULL);

		//guard
		if (numDevices == 0)
		{
			throw std::runtime_error("Found no physical GPUs");
		}

		//get devices
		_physicalGpus = std::vector<PhysicalGpu>{};
		_physicalGpus.resize(numDevices);
		std::vector<VkPhysicalDevice> physicalDevices = {};
		physicalDevices.resize(numDevices);

		vkEnumeratePhysicalDevices(_vkInstanceHandle, &numDevices, physicalDevices.data());

		for (size_t i = 0; i < physicalDevices.size(); i++)
		{
			_physicalGpus[i].physicalDevice = physicalDevices[i];
			//get queue families
			uint32_t numQueues = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(_physicalGpus[i].physicalDevice, &numQueues, NULL);

			//guard
			if (numQueues == 0)
			{
				throw std::runtime_error("No device queues found");
			}

			_physicalGpus[i].queueFamilyProps.resize(numQueues);
			vkGetPhysicalDeviceQueueFamilyProperties(_physicalGpus[i].physicalDevice, &numQueues, _physicalGpus[i].queueFamilyProps.data());

			//get device ext
			uint32_t numExtension = 0;
			vkEnumerateDeviceExtensionProperties(_physicalGpus[i].physicalDevice, nullptr, &numExtension, nullptr);
			//guard
			if (numExtension == 0)
			{
				throw std::runtime_error("No device extensions found");
			}

			_physicalGpus[i].extProps.resize(numExtension);
			vkEnumerateDeviceExtensionProperties(_physicalGpus[i].physicalDevice, nullptr, &numExtension, _physicalGpus[i].extProps.data());

			//get surface capas
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalGpus[i].physicalDevice, _vkSurface, &_physicalGpus[i].surfaceCapa);

			//get surface formats
			uint32_t numFormats;
			vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalGpus[i].physicalDevice, _vkSurface, &numFormats, NULL);

			if (numFormats == 0)
			{
				throw std::runtime_error("No surface formats");
			}

			_physicalGpus[i].surfaceFormats.resize(numFormats);
			vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalGpus[i].physicalDevice, _vkSurface, &numFormats, _physicalGpus[i].surfaceFormats.data());

			//get present modes
			uint32_t numPresentModes;
			vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalGpus[i].physicalDevice, _vkSurface, &numPresentModes, NULL);
			if (numPresentModes == 0)
			{
				throw std::runtime_error("No present modes");
			}

			_physicalGpus[i].presentModes.resize(numPresentModes);
			vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalGpus[i].physicalDevice, _vkSurface, &numPresentModes, _physicalGpus[i].presentModes.data());

			//get memory props
			vkGetPhysicalDeviceMemoryProperties(_physicalGpus[i].physicalDevice, &_physicalGpus[i].memProps);
			vkGetPhysicalDeviceProperties(_physicalGpus[i].physicalDevice, &_physicalGpus[i].deviceProps);
		}
	}

	void Renderer::CreateSwapchain()
	{
		AutoPickSurfaceFormat(&_selectedGpu->surfaceFormats);
		AutoPickPresentMode(&_selectedGpu->presentModes);
		UpdateSurfaceExtent();

		VkSwapchainCreateInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = _vkSurface;
		info.minImageCount = _NUM_FRAME_DATA;
		info.imageFormat = _vkSurfaceFormat.format;
		info.imageColorSpace = _vkSurfaceFormat.colorSpace;
		info.imageExtent = _extent;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if (_graphicsFamilyId != _presentFamilyId)
		{
			uint32_t indices[] = {
				static_cast<uint32_t>(_graphicsFamilyId),
				static_cast<uint32_t>(_presentFamilyId)
			};

			//do sharing
			info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = 2;
			info.pQueueFamilyIndices = indices;
		}
		else
		{
			//do exclusive
			info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = _vkPresentMode;

		info.clipped = VK_TRUE;

		vkCreateSwapchainKHR(_logicalDevice, &info, nullptr, &_vkSwapchain);

		//get swap images
		uint32_t numImages = 0;
		std::vector<VkImage> swapchainImages = std::vector<VkImage>(_NUM_FRAME_DATA);

		vkGetSwapchainImagesKHR(_logicalDevice, _vkSwapchain, &numImages, nullptr);
		//guard
		if (!(numImages > 0)) throw std::runtime_error("No swapchain images");

		vkGetSwapchainImagesKHR(_logicalDevice, _vkSwapchain, &numImages, swapchainImages.data());

		for (size_t i = 0; i < _NUM_FRAME_DATA; ++i)
		{
			VkImageViewCreateInfo imageViewInfo = {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = swapchainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = _vkSurfaceFormat.format;
			
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			//TODO remove magic number
			imageViewInfo.subresourceRange.baseMipLevel = 0;

			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			imageViewInfo.flags = 0;

			VkImageView imageView;
			vkCreateImageView(_logicalDevice, &imageViewInfo, nullptr, &imageView);

			Wombat::Image wombatImg;
			wombatImg.imageView = imageView;
			wombatImg.swapchainImage = swapchainImages[i];
			wombatImg.format = _vkSurfaceFormat.format;
			wombatImg.extent2D = _extent;

			_images.push_back(wombatImg);
		}
	}

	void Renderer::AutoSelectBestGpu()
	{
		std::cout << "Num of GPUs: " << _physicalGpus.size() << std::endl;

		for (size_t i = 0; i < _physicalGpus.size(); i++)
		{
			PhysicalGpu* gpu = &_physicalGpus[i];

			std::cout << "Checking GPU " << gpu->deviceProps.deviceName << std::endl;

			// This is again related to queues.  Don't worry I'll get there soon.
			int graphicsIdx = -1;
			int presentIdx = -1;

			// Remember when we created our instance we got all those device extensions?
			// Now we need to make sure our physical device supports them.
			bool supportsExt = HasPhysicalDeviceExtensionSupport(gpu, &_requiredVkDeviceExtProps);
			if (!supportsExt)
			{
				continue;
			}

			// No surface formats? =(
			if (gpu->surfaceFormats.size() == 0)
			{
				continue;
			}

			// No present modes? =(
			if (gpu->presentModes.size() == 0)
			{
				continue;
			}

			// Find graphics queue family
			for (int j = 0; j < gpu->queueFamilyProps.size(); ++j)
			{
				VkQueueFamilyProperties& props = gpu->queueFamilyProps[j];

				if (props.queueCount == 0)
				{
					continue;
				}

				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					// Got it!
					graphicsIdx = j;
					break;
				}
			}

			// Find present queue family
			for (int j = 0; j < gpu->queueFamilyProps.size(); ++j)
			{
				VkQueueFamilyProperties& props = gpu->queueFamilyProps[j];

				if (props.queueCount == 0)
				{
					continue;
				}

				// A rather perplexing call in the Vulkan API, but
				// it is a necessity to call.
				VkBool32 supportsPresent = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu->physicalDevice, j, _vkSurface, &supportsPresent);
				if (supportsPresent)
				{
					// Got it!
					presentIdx = j;
					break;
				}
			}

			// Did we find a device supporting both graphics and present.
			if (graphicsIdx >= 0 && presentIdx >= 0)
			{
				_graphicsFamilyId = graphicsIdx;
				_presentFamilyId = presentIdx;
				_selectedPhysicalDevice = &gpu->physicalDevice;
				_selectedGpu = gpu;
				std::cout << "Picking " << gpu->deviceProps.deviceName << std::endl;
				return;
			}
		} //end of loop

		//error if we got nothing
		throw std::runtime_error("No compatible GPU available");
	}

	void Renderer::CreateLogicalDeviceAndQueue()
	{
		std::vector<int32_t> uniqueIds;
		if (_presentFamilyId == _graphicsFamilyId)
		{
			uniqueIds.push_back(_presentFamilyId);
		}
		else
		{
			uniqueIds.push_back(_presentFamilyId);
			uniqueIds.push_back(_graphicsFamilyId);
		}

		std::vector<VkDeviceQueueCreateInfo> uniqueInfos;

		const float priority = 1.0f;
		for (size_t i = 0; i < uniqueIds.size(); i++)
		{
			VkDeviceQueueCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.queueFamilyIndex = uniqueIds[i];
			info.queueCount = 1;
			info.pQueuePriorities = &priority;

			uniqueInfos.push_back(info);
		}

		VkPhysicalDeviceFeatures deviceFeats = {};
		deviceFeats.textureCompressionBC = VK_TRUE;
		deviceFeats.imageCubeArray = VK_TRUE;
		deviceFeats.depthClamp = VK_TRUE;
		deviceFeats.depthBiasClamp = VK_TRUE;
		deviceFeats.depthBounds = VK_TRUE;
		deviceFeats.fillModeNonSolid = VK_TRUE;

		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.queueCreateInfoCount = static_cast<int32_t>(uniqueInfos.size());
		deviceInfo.pQueueCreateInfos = uniqueInfos.data();
		deviceInfo.pEnabledFeatures = &deviceFeats;
		deviceInfo.enabledExtensionCount = static_cast<int32_t>(_requiredVkDeviceExtProps.size());
		deviceInfo.ppEnabledExtensionNames = _requiredVkDeviceExtProps.data();

		if (_isDebugEnabled)
		{
			deviceInfo.enabledLayerCount = static_cast<int32_t>(_requiredVkValidationLayers.size());
			deviceInfo.ppEnabledLayerNames = _requiredVkValidationLayers.data();
		}
		else
		{
			deviceInfo.enabledLayerCount = 0;
		}

		try
		{
			vkCreateDevice(*_selectedPhysicalDevice, &deviceInfo, nullptr, &_logicalDevice);

		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		try
		{
			vkGetDeviceQueue(_logicalDevice, _graphicsFamilyId, 0, &_vkGraphicsQueue);

		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;

		}
		vkGetDeviceQueue(_logicalDevice, _presentFamilyId, 0, &_vkPresentQueue);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			std::cout << "Destroying debugger" << std::endl;
			func(instance, debugMessenger, pAllocator);
		}
		else
		{
			std::cout << "No vkDestroyDebugUtilsMessengerEXT" << std::endl;
		}
	}
} // namespace Wombat
