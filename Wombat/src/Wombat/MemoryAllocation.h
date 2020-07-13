#pragma once

#include <vulkan/vulkan.h>

namespace Wombat
{
	class MemoryAllocation
	{
		/*
			pAllocateInfo->allocationSize must be less than or equal to VkPhysicalDeviceMemoryProperties::memoryHeaps[memindex].size where memindex = VkPhysicalDeviceMemoryProperties::memoryTypes[pAllocateInfo->memoryTypeIndex].heapIndex as returned by vkGetPhysicalDeviceMemoryProperties for the VkPhysicalDevice that device was created from

			pAllocateInfo->memoryTypeIndex must be less than VkPhysicalDeviceMemoryProperties::memoryTypeCount as returned by vkGetPhysicalDeviceMemoryProperties for the VkPhysicalDevice that device was created from

			If the deviceCoherentMemory feature is not enabled, pAllocateInfo->memoryTypeIndex must not identify a memory type supporting VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD


			typedef enum VkMemoryPropertyFlagBits {
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
				VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
				VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
				VK_MEMORY_PROPERTY_PROTECTED_BIT = 0x00000020,
				VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD = 0x00000040,
				VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD = 0x00000080,
			} VkMemoryPropertyFlagBits;


			////////////FOR THE BUFFER PART OF THE ALLOCATION////////////////
			typedef struct VkBufferCreateInfo {
				VkStructureType        sType;
				const void*            pNext;
				VkBufferCreateFlags    flags;
				VkDeviceSize           size;
				VkBufferUsageFlags     usage;
				VkSharingMode          sharingMode;
				uint32_t               queueFamilyIndexCount;
				const uint32_t*        pQueueFamilyIndices;
			} VkBufferCreateInfo;
		*/
	public:
		MemoryAllocation();
		~MemoryAllocation();

		 VkDeviceMemory* deviceHandle; //maxMemoryAllocationCount 
		 VkDeviceSize offset;
		 VkDeviceSize size; //VkPhysicalDeviceMaintenance3Properties::maxMemoryAllocationSize
	};
}