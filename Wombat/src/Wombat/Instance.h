#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Wombat
{
    class Instance
    {
        public:
            Instance::Instance();
            Instance::~Instance();

        private:
            VkInstance _vkInstance;
    };
}