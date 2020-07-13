#include "RendererOptionsComponent.hpp"

namespace Wombat::Rendering
{
    RendererOptionsComponent::RendererOptionsComponent(bool isDebugEnabled, const char *appTitle)
    {
        _isDebugEnabled = isDebugEnabled;
        _appTitle = appTitle;
    }

    RendererOptionsComponent::~RendererOptionsComponent()
    {
    }

} // namespace Wombat::Rendering
