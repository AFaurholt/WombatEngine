#include <iostream>
#include "Instance.h"

namespace Wombat
{
    Instance::Instance(const char* appName, bool isDebugEnabled)
    {
        _renderer = Renderer(appName, isDebugEnabled);
    }

    Instance::~Instance()
    {
        delete &_renderer;
    }

    void Instance::OpenDebugWindow()
    {
        _renderer.OpenDebugWindow();
    }
}