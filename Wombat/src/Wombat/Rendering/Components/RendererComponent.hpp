#include "vulkan/vulkan.hpp"

#include "RendererOptionsComponent.hpp"

namespace Wombat::Rendering
{
    class RendererComponent
    {
    public:
        RendererOptionsComponent _rendererOpts;
        //Vulkan specialization
        VkInstance _vkInstance;
    };
    
    
    
} // namespace Wombat::Rendering
