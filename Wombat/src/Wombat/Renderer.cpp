#include "Renderer.h"

namespace Wombat
{
    Renderer::Renderer(const char* appName, bool isDebugEnabled = true)
    {
        //guard for GLFW
        //TODO figure out what to do if GLFW not used
        if (!glfwInit()) throw std::runtime_error("GLFW failed to init");

       Renderer::CreateInstance(appName, isDebugEnabled);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _glfwDebugWindow = glfwCreateWindow(640, 480, "Wombat Debug", NULL, NULL);
        if(glfwCreateWindowSurface(_vkInstanceHandle, _glfwDebugWindow, NULL, &_vkSurface) != VK_SUCCESS)
            throw std::runtime_error("GLFW failed to create surface");

        

    }

    Renderer::~Renderer()
    {
        glfwTerminate();

        vkDestroySurfaceKHR(_vkInstanceHandle, _vkSurface, nullptr);
        vkDestroyInstance(_vkInstanceHandle, nullptr);
    }

     std::vector<VkLayerProperties> Renderer::GetValidationLayerProperties()
     {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        return availableLayers;
     }

     bool Renderer::CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
                                            std::vector<const char*> requestedLayers)
     {
         //for each requested layer
         for(const char* requestedLayer : requestedLayers)
         {
             bool hasRequested = false;

             //for each available layer
             for(const auto& layerProperties : availableLayers)
             {
                 if (strcmp(requestedLayer, layerProperties.layerName) == 0)
                 {
                     hasRequested = true;
                     break;
                 }
             }

            //guard
             if(!hasRequested) return false;
         }

         //if we're not returned early, all requested layers exist
         return true;
     }

     void Renderer::CreateInstance(const char* appName, bool isDebugEnabled)
     {
        //TODO move into config module
        VkApplicationInfo vkAppInfo{};
        vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.pEngineName = "WombatEngine";
        vkAppInfo.apiVersion = VK_API_VERSION_1_2;
        vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        vkAppInfo.pApplicationName = appName;

        const std::vector<const char*> requestedLayers = 
            {
                "VK_LAYER_KHRONOS_validation"
            };
        //TODO move into config module
        VkInstanceCreateInfo vkInstanceInfo{};
        vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkInstanceInfo.pApplicationInfo = &vkAppInfo;
        //validation layers
        if (!isDebugEnabled)
        {
            vkInstanceInfo.enabledLayerCount = 0;
        }
        else
        {
            //TODO move this into config
            

            bool allRequestedLayersAvailable = CheckAllLayersSupported(GetValidationLayerProperties(), requestedLayers);
            if(!allRequestedLayersAvailable)
            {
                throw std::runtime_error("One or more requested validation layers not available.");
            }

            vkInstanceInfo.enabledLayerCount = static_cast<uint32_t>(requestedLayers.size());
            vkInstanceInfo.ppEnabledLayerNames = requestedLayers.data();
        }
        //extensions
        //TODO figure out what to do if GLFW is not used
        uint32_t extensionCount = 1;
        std::vector<const char*> extensionNames = {"VK_KHR_surface"};
        // const char** extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
        vkInstanceInfo.enabledExtensionCount = extensionCount;
        vkInstanceInfo.ppEnabledExtensionNames = extensionNames.data();

        vkCreateInstance(&vkInstanceInfo, nullptr, &_vkInstanceHandle);
        
     }

     void Renderer::OpenDebugWindow()
     {
         while (!glfwWindowShouldClose(_glfwDebugWindow))
        {
            glfwPollEvents();
        }

     }
} // namespace Wombat
