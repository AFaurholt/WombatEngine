
#include "Instance.h"

namespace Wombat
{
    Instance::Instance(const char* appName, bool isDebugEnabled)
    {
        std::cout << "Instance ctor" << std::endl;

        _pRenderer = new Renderer(appName, isDebugEnabled);
    }

    Instance::~Instance()
    {
        std::cout << "Instance destruction" << std::endl;


        delete _pRenderer;
    }

    void Instance::OpenDebugWindow()
    {
        _pRenderer->OpenDebugWindow();
    }
}