#pragma once
#include <vulkan/vulkan.hpp>

namespace Wombat::Rendering
{
	struct GpuWithInfoComponent
	{
		VkPhysicalDevice						physicalDevice;
		VkSurfaceCapabilitiesKHR				surfaceCapabilities;
		VkPhysicalDeviceMemoryProperties2		memoryProperties2;
		VkPhysicalDeviceProperties2				physicalDeviceProperties2;
		std::vector<VkQueueFamilyProperties2>	queueFamilyProperties{};
		std::vector<VkExtensionProperties>		extensionProperties{};
		std::vector<VkSurfaceFormatKHR>			surfaceFormats{};
		std::vector<VkPresentModeKHR>			presentModes{};
	};
}