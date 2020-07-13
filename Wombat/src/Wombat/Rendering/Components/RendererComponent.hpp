#include "RendererOptionsComponent.hpp"

namespace Wombat::Rendering
{
    class RendererComponent
    {
    private:
        /* data */
        RendererOptionsComponent _rendererOpts;
        //Vulkan specialization
        VkInstance _vkInstance;
        

    public:
        RendererComponent(/* args */);
        ~RendererComponent();
    };
    
    
    
} // namespace Wombat::Rendering
