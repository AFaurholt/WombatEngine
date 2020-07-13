#pragma once

#include <vulkan/vulkan.h>

#include <Wombat\MemoryBlock.h>
#include <Wombat\MemoryAllocation.h>

namespace Wombat
{
	class MemoryPool
	{
	public:
		MemoryPool(uint16_t memoryType);
		MemoryPool(uint16_t memoryType, VkDeviceSize size);
		~MemoryPool();

		const uint16_t orderZeroSize;

	private:
		Wombat::MemoryBlock topBlock_;
		Wombat::MemoryAllocation backingAllocation_;
		uint64_t blockCount_;
		uint16_t memoryType_;

        // Find a memory in `memoryTypeBitsRequirement` that includes all of `requiredProperties`
        int32_t FindMemoryWithProperties(const VkPhysicalDeviceMemoryProperties* pMemoryProperties,
            uint32_t memoryTypeBitsRequirement,
			VkMemoryPropertyFlags requiredProperties);

		void AllocatePool(uint16_t memoryType, VkDeviceSize size);

	};
}