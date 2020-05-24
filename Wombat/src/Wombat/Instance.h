#pragma once
#include "Renderer.h"

#include <vector>
#include <iostream>

namespace Wombat
{
    /*
    Takes care of any init, generating resources, taking care of resource life-time
    */
    class Instance
    {
        public:
            Instance::Instance(const char* appName, bool isDebugEnabled);
            Instance::~Instance();
            void OpenDebugWindow();

        private:
            Wombat::Renderer* _pRenderer;


    };
} //namespace Wombat