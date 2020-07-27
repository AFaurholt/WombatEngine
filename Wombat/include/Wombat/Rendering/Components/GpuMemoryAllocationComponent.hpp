#pragma once
#include "vulkan/vulkan.hpp"

namespace Wombat::Rendering
{
    struct GpuMemoryAllocationComponent
    {
        //use IEC bytes 1024
        //256 MB per DEVICE_LOCAL allocation
        VkDeviceSize standardSize{};
        VkDeviceSize size;
        VkDeviceSize orderZeroSize;
    };
    
} // namespace Wombat::Rendering
