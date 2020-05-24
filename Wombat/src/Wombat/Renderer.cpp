#include "Renderer.h"

namespace Wombat
{

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
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

	Renderer::Renderer(const char* appName, bool isDebugEnabled = true)
	{
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

#ifdef WB_PLATFORM_WINDOWS
		_requiredVkInstanceExtProps.push_back("VK_KHR_win32_surface");
#endif // WB_PLATFORM_WINDOWS


		for (size_t i = 0; i < _requiredVkInstanceExtProps.size(); i++)
		{
			std::cout << _requiredVkInstanceExtProps[i] << std::endl;
		}

		_requiredVkDeviceExtProps =
		{
			"VK_KHR_SWAPCHAIN_EXTENSION_NAME" };

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


		// void *obsModule = GET_MODULE(L"graphics-hook64.dll");
		// if (obsModule != nullptr)
		// {
		//     FREE_MODULE((void *)obsModule);
		// }

		try
		{
			vkDestroySurfaceKHR(_vkInstanceHandle, _vkSurface, nullptr);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

		try
		{
			vkDestroyInstance(_vkInstanceHandle, nullptr);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

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

	bool Renderer::CheckPhysicalDeviceExtensionSupport(PhysicalGpu gpu,
		std::vector<const char*> requiredDeviceExtProps)
	{
		//for each requested ext
		for (const char* requiredDeviceExtProp : requiredDeviceExtProps)
		{
			bool hasRequested = false;

			//for each available gpu ext
			for (const auto& gpuDeviceProp : gpu.extProps)
			{
				if (strcmp(requiredDeviceExtProp, gpuDeviceProp.extensionName) == 0)
				{
					hasRequested = true;
					break;
				}
			}

			//guard
			if (!hasRequested)
				return false;
		}

		//if we're not returned early, all requested ext exist
		return true;
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

		VkResult vkResult = vkCreateInstance(&vkInstanceInfo, nullptr, &_vkInstanceHandle);
		if (vkResult != VK_SUCCESS)
			ThrowErrorWithVkResult("Failed to create VK instance ", vkResult);
	}

	void Renderer::ThrowErrorWithVkResult(const char* errMsg, VkResult vkResult)
	{
		std::string err = errMsg + std::to_string(vkResult);
		throw std::runtime_error(err);
	}

	void Renderer::OpenDebugWindow()
	{
		try
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			_glfwDebugWindow = glfwCreateWindow(640, 480, "Wombat Debug", NULL, NULL);
			VkResult vkResult = glfwCreateWindowSurface(_vkInstanceHandle, _glfwDebugWindow, NULL, &_vkSurface);
			if (vkResult != VK_SUCCESS)
			{
				ThrowErrorWithVkResult("GLFW failed to create surface ", vkResult);
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

	void Renderer::UpdatePhysicalDeviceList()
	{
		uint32_t numDevices;
		vkEnumeratePhysicalDevices(_vkInstanceHandle, &numDevices, NULL);

		//guard
		if (numDevices == 0)
		{
			throw std::runtime_error("Found no GPUs");
		}

		//get devices
		_physicalGpus.resize(numDevices);
		vkEnumeratePhysicalDevices(_vkInstanceHandle, &numDevices, &_physicalGpus.data()->physicalDevice);

		for (size_t i = 0; i < _physicalGpus.size(); i++)
		{
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
			uint32_t numExtension;
			vkEnumerateDeviceExtensionProperties(_physicalGpus[i].physicalDevice, NULL, &numExtension, NULL);
			//guard
			if (numExtension == 0)
			{
				throw std::runtime_error("No device extensions found");
			}

			_physicalGpus[i].extProps.resize(numExtension);
			vkEnumerateDeviceExtensionProperties(_physicalGpus[i].physicalDevice, NULL, &numExtension, _physicalGpus[i].extProps.data());

			//get surface caps
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

	/*
	Assigns graphics family, present family, gpu and physical device automatically
	*/
	void Renderer::AutoSelectBestGpu()
	{
		for (size_t i = 0; i < _physicalGpus.size(); i++)
		{
			PhysicalGpu* gpu = &_physicalGpus[i];

			// This is again related to queues.  Don't worry I'll get there soon.
			int graphicsIdx = -1;
			int presentIdx = -1;

			// Remember when we created our instance we got all those device extensions?
			// Now we need to make sure our physical device supports them.
			if (!CheckPhysicalDeviceExtensionSupport(*gpu, _requiredVkDeviceExtProps))
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
				return;
			}
		} //end of loop

		//error if we got nothing
		throw std::runtime_error("No compatible GPU available");
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

} // namespace Wombat
