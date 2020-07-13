#pragma once

#include <vulkan/vulkan.h>

namespace Wombat
{
	class MemoryBlock
	{
	public:
		MemoryBlock(uint16_t _sizeOrder, VkDeviceSize _size, void* _pData);
		MemoryBlock(uint16_t _sizeOrder, VkDeviceSize _size, void* _pData, MemoryBlock* _pParent);
		~MemoryBlock();

		uint16_t sizeOrder;
		MemoryBlock* pParent;
		MemoryBlock* pChildLeft;
		MemoryBlock* pChildRight;
		void* pData; //pointer minus offset must be aligned to at least VkPhysicalDeviceLimits::minMemoryMapAlignment.
		VkDeviceSize size;

	};
}