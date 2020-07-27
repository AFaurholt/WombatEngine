#include "vulkan/vulkan.hpp"

#include "RendererOptionsComponent.hpp"

namespace Wombat::Rendering
{
    struct RendererComponent
    {
        RendererOptionsComponent rendererOpts;
        //Vulkan specialization
        VkInstance rendererInstance;
    };
} // namespace Wombat::Rendering
