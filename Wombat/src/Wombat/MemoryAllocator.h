#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "MemoryBlock.h"
#include "MemoryAllocation.h"
#include <Wombat\MemoryPool.h>

namespace Wombat
{
	class MemoryAllocator
	{
	public:
		MemoryAllocator();
		~MemoryAllocator();

		Wombat::MemoryBlock* AllocateBlock();
		void Cleanup();
		
	private:
		std::vector<MemoryPool> memoryPools_{};

		void TryAllocateToPool();
	};
}