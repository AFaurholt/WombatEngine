#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace Wombat
{
    struct PhysicalGpu
    {
        public:
        VkPhysicalDevice physicalDevice;
        std::vector<VkQueueFamilyProperties> queueFamilyProps;
        std::vector<VkExtensionProperties> extProps;
        VkSurfaceCapabilitiesKHR surfaceCapa;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
        VkPhysicalDeviceMemoryProperties memProps;
        VkPhysicalDeviceProperties deviceProps;
    };
    
} // namespace Wombat
