#include "vulkan/vulkan.hpp"

namespace Wombat::Rendering
{
    //TODO make template
    struct SurfaceComponent
    {
        //VK specialization
        VkSurfaceKHR _vkSurfaceKHR;
        VkSurfaceFormatKHR _vkSurfaceFormatKHR;    
    };
} // namespace Wombat::Rendering
