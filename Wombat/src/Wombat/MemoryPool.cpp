#include "MemoryPool.h"

// Find a memory in `memoryTypeBitsRequirement` that includes all of `requiredProperties`

int32_t Wombat::MemoryPool::FindMemoryWithProperties(const VkPhysicalDeviceMemoryProperties* pMemoryProperties, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties)
{
	const uint32_t memoryCount = pMemoryProperties->memoryTypeCount;

	for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex)
	{
		const uint32_t memoryTypeBits = (1 << memoryIndex);
		const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

		const VkMemoryPropertyFlags properties = pMemoryProperties->memoryTypes[memoryIndex].propertyFlags;
		const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

		if (isRequiredMemoryType && hasRequiredProperties)
		{
			return static_cast<int32_t>(memoryIndex);
		}
	}

	// failed to find memory type
	return -1;
}
