namespace Wombat::Rendering
{
    //TODO make template
    class SurfaceComponent
    {
    private:
        /* data */
        //VK specialization
        VkSurfaceKHR _vkSurfaceKHR;
        VkSurfaceFormatKHR _vkSurfaceFormatKHR;

    public:
        SurfaceComponent(/* args */);
        ~SurfaceComponent();
    };
    
    SurfaceComponent::SurfaceComponent(/* args */)
    {
    }
    
    SurfaceComponent::~SurfaceComponent()
    {
    }
    
} // namespace Wombat::Rendering
