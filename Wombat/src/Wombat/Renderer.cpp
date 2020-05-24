#include "Renderer.h"

namespace Wombat
{
    Renderer::Renderer(const char *appName, bool isDebugEnabled = true)
    {
        _requiredVkValidationLayers =
            {
                "VK_LAYER_KHRONOS_validation"};

        std::cout << "Renderer ctor" << std::endl;

        //guard for GLFW
        //TODO figure out what to do if GLFW not used
        if (!glfwInit())
            throw std::runtime_error("GLFW failed to init");

        //TODO figure out what to do if GLFW is not used
        uint32_t extensionCount = 0;
        // std::vector<const char*> extensionNames = {"VK_KHR_surface"};
        const char **extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
        _requiredVkInstanceExtProps = std::vector<const char *>{*extensionNames};

        _requiredVkDeviceExtProps =
            {
                "VK_KHR_SWAPCHAIN_EXTENSION_NAME"};

        Renderer::CreateInstance(appName, isDebugEnabled);
    }

    Renderer::~Renderer()
    {
        std::cout << "Renderer destruction" << std::endl;
        glfwTerminate();

        vkDestroySurfaceKHR(_vkInstanceHandle, _vkSurface, nullptr);
        vkDestroyInstance(_vkInstanceHandle, nullptr);
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
                                           std::vector<const char *> requestedLayers)
    {
        //for each requested layer
        for (const char *requestedLayer : requestedLayers)
        {
            bool hasRequested = false;

            //for each available layer
            for (const auto &layerProperties : availableLayers)
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
                                                       std::vector<const char *> requiredDeviceExtProps)
    {
        //for each requested ext
        for (const char *requiredDeviceExtProp : requiredDeviceExtProps)
        {
            bool hasRequested = false;

            //for each available gpu ext
            for (const auto &gpuDeviceProp : gpu.extProps)
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

    void Renderer::CreateInstance(const char *appName, bool isDebugEnabled)
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

        vkCreateInstance(&vkInstanceInfo, nullptr, &_vkInstanceHandle);
    }

    void Renderer::OpenDebugWindow()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _glfwDebugWindow = glfwCreateWindow(640, 480, "Wombat Debug", NULL, NULL);
        if (glfwCreateWindowSurface(_vkInstanceHandle, _glfwDebugWindow, NULL, &_vkSurface) != VK_SUCCESS)
            throw std::runtime_error("GLFW failed to create surface");

        while (!glfwWindowShouldClose(_glfwDebugWindow))
        {
            glfwPollEvents();
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
            PhysicalGpu *gpu = &_physicalGpus[i];

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
                VkQueueFamilyProperties &props = gpu->queueFamilyProps[j];

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
                VkQueueFamilyProperties &props = gpu->queueFamilyProps[j];

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
} // namespace Wombat
